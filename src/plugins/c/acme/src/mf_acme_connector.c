#include <stdlib.h> /* malloc */
#include <iio.h>

/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_types.h"
#include "publisher.h"
#include "mf_acme_connector.h"

#define SUCCESS 1
#define FAILURE 0
#define MAX_DEVICES 8
#define MAX_CHANNELS 5
#define ACME_MAX_STR_LEN 64
#define SAMPLES_PER_READ 16

/*******************************************************************************
 * VARIABLE DECLARATIONS
 ******************************************************************************/
/*
 * Trivial helper structure for a scan_element.
 */
struct my_channel {
	char label[128];
	char unit[128];
	double scale;
	double value;		/* average value for one buffer */
	const struct iio_channel *iio;
};

static struct iio_context *ctx;
struct iio_device **devices;
struct iio_buffer **buffer;
static long long sampling_freq[MAX_DEVICES];
static unsigned int nb_devices;
static unsigned int nb_channels;
static long long nb_samples[MAX_DEVICES];
static struct my_channel my_chn[MAX_DEVICES][MAX_CHANNELS];

/*******************************************************************************
 * FORWARD DECLARATIONS
 ******************************************************************************/
static void init_ina2xx_channels(struct iio_device *dev, int device_idx);
int create_EventSets(ACME_Plugin *data, char **acme_events, size_t num_events);
static inline int chan_device(const struct iio_channel *chn);
static inline int chan_index(const struct iio_channel *chn, int device_idx);
static ssize_t print_sample(const struct iio_channel *chn, void *buf, size_t len, void *d);
void filter(ACME_Plugin *data);
void reset_my_channels_value(void);

/* Initialize the ACME plug-in */
int
mf_acme_init(ACME_Plugin *data, char **acme_events, size_t num_events)
{
	if(!mf_acme_is_enabled()) {
		return FAILURE;
	}

	if(!create_EventSets(data, acme_events, num_events)) {
		return FAILURE;
	}

	int ret = unit_file_check("acme");
    if (ret < 0) {
        return FAILURE;
    }
    else if(ret == 0) {
    	mf_acme_unit_init();
    }
	return SUCCESS;
}

/* Checks if the ACME component is available through network
 * if ACME component is available initialize all channels for all 
 * devices, collects all events units and send them to mf_server */
int
mf_acme_is_enabled()
{
	int c, device_idx;
	char temp[1024];
	unsigned int buffer_size = SAMPLES_PER_READ;

	ctx = iio_create_network_context("power-nvidia-0");
	if (!ctx) {
		fprintf(stderr, "Unable to create IIO context\n");
		return FAILURE;
	}

	/* count the number of devices */
	nb_devices = iio_context_get_devices_count(ctx);
	if(nb_devices > MAX_DEVICES) {
		fprintf(stderr, "Too many devices than maximum setting\n");
		return FAILURE;	
	}
	/* allocate devices */
	devices = (struct iio_device **)malloc(nb_devices * sizeof(struct iio_device *));

	/* allocate different buffers for different devices */
	buffer = (struct iio_buffer **)malloc(nb_devices * sizeof(struct iio_buffer *));

	for(device_idx = 0; device_idx < nb_devices; device_idx++) {

		devices[device_idx] = iio_context_get_device(ctx, device_idx);
		
		if(!devices[device_idx]) {
			iio_context_destroy(ctx);
			fprintf(stderr, "Device %d is not enabled.\n", device_idx);
			return FAILURE;
		}

		if(iio_device_attr_write(devices[device_idx], "in_oversampling_ratio", "4") <= 0)
		{
			fprintf(stderr, "write ratio 4 failed.\n");
			return FAILURE;
		}

		memset(temp, '0', 1024);
		c = iio_device_attr_read(devices[device_idx], "in_sampling_frequency", temp, 1024);
		if (c) {
			sampling_freq[device_idx] = atoi(temp);
		}
		else
			return FAILURE;
		
		/* initialize my_chn variables for sampling */
		init_ina2xx_channels(devices[device_idx], device_idx);

		buffer[device_idx] = iio_device_create_buffer(devices[device_idx], buffer_size, false);

		if (!buffer[device_idx]) {
			fprintf(stderr, "Unable to allocate buffer\n");
			iio_context_destroy(ctx);
			return FAILURE;
		}
	}
	return SUCCESS;
}

/* initialize the units of all events */
void
mf_acme_unit_init()
{
	int i, j, idx;

	/* allocate metrics units variables */
	metric_units *ACME_units = malloc(sizeof(metric_units));

    memset(ACME_units, 0, sizeof(metric_units));

    idx = 0;
    for(i = 0; i < nb_devices; i++) {
    	for(j = 0; j < nb_channels; j++) {
    		ACME_units->metric_name[idx] = malloc(32 * sizeof(char));
            strncpy(ACME_units->metric_name[idx], my_chn[i][j].label, sizeof(char)*32);
            ACME_units->plugin_name[idx] = malloc(32 * sizeof(char));
            strncpy(ACME_units->plugin_name[idx], "mf_plugin_acme", sizeof(char)*32);
            ACME_units->unit[idx] = malloc(4 * sizeof(char));
            strncpy(ACME_units->unit[idx], my_chn[i][j].unit, sizeof(char)*4);
            idx++;
    	}
    }
    ACME_units->num_metrics = idx;

    publish_unit(ACME_units);
}

/* Refills the buffer, reads all samples inside the buffer, and filter the desired acme events */
int
mf_acme_sample(ACME_Plugin *data)
{
	int device_idx;

	/* for each device refill the buffer and reads all samples */
	for(device_idx = 0; device_idx < nb_devices; device_idx++) {
    	int ret = iio_buffer_refill(buffer[device_idx]);
		if (ret < 0) {
			fprintf(stderr, "Unable to refill buffer: %s\n", strerror(-ret));
			return FAILURE;
		}
		iio_buffer_foreach_sample(buffer[device_idx], print_sample, NULL);
	}

	/* filters the user specified data */
	filter(data);
	
	/* my_chn values of all channels are reset to zeros */
	reset_my_channels_value();
	return SUCCESS;
}

/* Conversion of samples data to a JSON document */
char*
mf_acme_to_json(ACME_Plugin *data)
{
    char *metric = malloc(METRIC_LENGTH_MAX * sizeof(char));
    char *json = malloc(JSON_LENGTH_MAX * sizeof(char));
    strcpy(json, "\"type\":\"acme-iio\"");

    int idx;
    size_t size = data->num_events;
    for (idx = 0; idx < size; ++idx) {
        sprintf(metric, ",\"%s\":%.4f", data->events[idx], data->values[idx]);
        strcat(json, metric);
    }
    free(metric);
    return json;
}

/* Stop and clean-up the acme plugin */
void
mf_acme_shutdown()
{
	int i;
	for (i=0; i<nb_devices; i++) {
		iio_buffer_destroy(buffer[i]);
		free(devices[i]);	
	}
	free(buffer);
	free(devices);
	iio_context_destroy(ctx);
}

/* Initialze the my_chn variable for each device, reads for each channel the scale, units, label,
   and all channels found are enabled. */
static void init_ina2xx_channels(struct iio_device *dev, int device_idx)
{
	int i;
	char buf[1024];
	struct iio_channel *ch;

	if (strcmp(iio_device_get_name(dev), "ina226")) {
		fprintf(stderr, "Unknown device %s\n",
			iio_device_get_name(dev));
		exit(-1);
	}
	nb_samples[device_idx] = 0;
	nb_channels = iio_device_get_channels_count(dev);

	/* FIXME: dyn alloc */
	if(nb_channels > MAX_CHANNELS){
		fprintf(stderr, "Too many channels.\n");
		exit(-1);
	}

	for (i = 0; i < nb_channels; i++) {
		const char *id;
		ch = iio_device_get_channel(dev, i);

		my_chn[device_idx][i].value = 0.0;
		my_chn[device_idx][i].iio = ch;

		id = iio_channel_get_id(ch);

		if (iio_channel_attr_read(ch, "scale", buf, sizeof(buf)) >= 0)
			my_chn[device_idx][i].scale = atof(buf);
		else
			my_chn[device_idx][i].scale = 1.0;

		if (!strncmp(id, "power", 5)) {
			sprintf(my_chn[device_idx][i].label,"iio:device%d:power",device_idx);
			strcpy(my_chn[device_idx][i].unit, "mW");

		} else if (!strncmp(id, "current", 6)) {
			sprintf(my_chn[device_idx][i].label,"iio:device%d:current",device_idx);
			strcpy(my_chn[device_idx][i].unit, "mA");

		} else if (!strncmp(id, "voltage1", 8)) {
			sprintf(my_chn[device_idx][i].label,"iio:device%d:vbus",device_idx);
			strcpy(my_chn[device_idx][i].unit, "mV");

		} else if (!strncmp(id, "voltage0", 8)) {
			sprintf(my_chn[device_idx][i].label,"iio:device%d:vshunt",device_idx);
			strcpy(my_chn[device_idx][i].unit, "mV");

		}
		iio_channel_enable(ch);
	}
}

/* According to the user's configuration and channels available, create a EventSet */
int create_EventSets(ACME_Plugin *data, char **acme_events, size_t num_events)
{
	int i, device_idx, channel_idx;
	int idx = 0;

	for (i = 0; i < num_events; i++) {
		for(device_idx = 0; device_idx < nb_devices; device_idx++) {
			for (channel_idx = 0; channel_idx < nb_channels; channel_idx++) {
				if(!strcmp(my_chn[device_idx][channel_idx].label, acme_events[i])) {
					/*input event is found*/
					data->events[idx] = malloc(ACME_MAX_STR_LEN + 1);
    	        	strcpy(data->events[idx], acme_events[i]);
        	    	idx++;
            		break;
				}
			}
			if(channel_idx < nb_channels)
				break;
		}
		if(device_idx >= nb_devices) {
			fprintf(stderr, "Event %s is not found.\n", acme_events[i]);
			return FAILURE;
		}
	}
	
	data->num_events = idx;
	return SUCCESS;
}

/* get the channel index for a channel */
static inline int chan_index(const struct iio_channel *chn, int device_idx)
{
	int i;

	for (i = 0; i < nb_channels; i++)
		if (my_chn[device_idx][i].iio == chn)
			return i;
	return -1;
}

/* get the device index for a channel */
static inline int chan_device(const struct iio_channel *chn)
{
	int i;
	const struct iio_device *dev;
	char tmp[64];

	dev = iio_channel_get_device(chn);
	const char * device_id = iio_device_get_id(dev);

	for (i = 0; i < nb_devices; i++) {
		memset(tmp, '0', 64);
		sprintf(tmp, "iio:device%d", i);
		if(!strcmp(device_id, tmp)) {
			return i;
		}
	}
	return -1;
}

/* callback function for each sample inside a buffer, 
 * values of the same channel are aggregated, and averaged overall one buffer */
static ssize_t print_sample(const struct iio_channel *chn, void *buf, size_t len, void *d)
{
	int val = (int)*(short *)buf;
	int vabs = 0;
	int i, j;

	i = chan_device(chn);	//i is device id
	j = chan_index(chn, i);	//j is channel id

	/* increment the sample count on the first channel */
	if(!j){
		nb_samples[i]++;
	}

	vabs =abs(val);
	my_chn[i][j].value += ((double)(vabs) - my_chn[i][j].value) / nb_samples[i];
	
	return len;
}

/* filter out the user specifed data, store the values in ACME_plugin data */
void filter(ACME_Plugin *data)
{
	int idx, device_idx, channel_idx;

	for (idx = 0; idx < data->num_events; idx++) {
			for (device_idx = 0; device_idx < nb_devices; device_idx++) {
				for (channel_idx = 0; channel_idx < nb_channels; channel_idx++) {
					if(!strcmp(data->events[idx], my_chn[device_idx][channel_idx].label)) {
						data->values[idx] = 
							my_chn[device_idx][channel_idx].value * my_chn[device_idx][channel_idx].scale;
            			break;
					}
				}
				if(channel_idx < nb_channels)
					break;
			}
			if(device_idx >= nb_devices) {
				fprintf(stderr, "event %s is not sampled\n", data->events[idx]);
			}	
		}
}

/* my_chn values are reset to zero, this function is called after all buffers are refilled, and samples are filtered */
void reset_my_channels_value()
{
	int device_idx, channel_idx;

	for (device_idx = 0; device_idx < nb_devices; device_idx++) {
		nb_samples[device_idx] = 0;
		for (channel_idx = 0; channel_idx < nb_channels; channel_idx++) {
			my_chn[device_idx][channel_idx].value = 0.0;
		}
	}
}