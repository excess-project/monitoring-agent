/*
 * Copyright (C) 2014-2015 University of Stuttgart
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h> /* malloc */
#include <time.h>
#include <string.h>
/* monitoring-related includes */
#include "mf_debug.h"
#include "mf_types.h"
#include "publisher.h"
#include "mf_movidius_arduino_connector.h"

#define SUCCESS 1
#define FAILURE 0

/*******************************************************************************
 * VARIABLE DECLARATIONS
 ******************************************************************************/
static int is_initialized = 0;
static int maximum_number_of_cores = 1;
static long long *before_time, *after_time;
static double* channel_coef=NULL;
static int arduino_fd = 0;
static char* arduino_buf = NULL;
static int arduino_buf_size = 2048;
MOVI_Plugin *all_data;

/*******************************************************************************
 * FORWARD DECLARATIONS
 ******************************************************************************/
static int is_movi_initialized();
static int create_eventset_for(MOVI_Plugin *data, char **movi_events, size_t num_events);
static int create_eventset_for_all(MOVI_Plugin *all_data);
void set_channel_coef();
int mf_movi_init(MOVI_Plugin *data, char **movi_events, size_t num_events);
void filter(MOVI_Plugin *all_data, MOVI_Plugin *data);
double timer_get_time(void);
int open_interfase(int* fd, const char* port_name);
int set_interface_attribs(int fd, int speed, int parity);
void set_blocking(int fd, int should_block);
void convert(MOVI_Plugin *data, char* arduino_output, size_t length, double* ch_coef1);
void read_arduino(int fd, char* arduino_output, size_t* length, size_t max_length);

/* Initializes the MOVI plug-in */
int mf_movi_init(MOVI_Plugin *data, char **movi_events, size_t num_events)
{
    if (is_movi_initialized()) {
        return SUCCESS;
    }
    int num_cores = 1;
    maximum_number_of_cores = num_cores;
    channel_coef = malloc(MOVI_MAX_PRESET_EVENTS * sizeof(double));
    set_channel_coef();

    all_data = malloc(num_cores * sizeof(MOVI_Plugin));
    create_eventset_for_all(all_data);
    mf_movi_unit_init();
    create_eventset_for(data, movi_events, num_events);
     /*
     * initialize time measurements
     */
    before_time = malloc(sizeof(double) * num_cores);
    after_time = malloc(sizeof(double) * num_cores);
     /*
      * open serial interfcae to arduino
      */
     const char *portname = "/dev/ttyACM0";
     if(open_interfase(&arduino_fd,portname)!=SUCCESS)
     {
       portname = "/dev/ttyACM1";
       if(open_interfase(&arduino_fd,portname)!=SUCCESS)
       {
         log_info ("error %d by opening %s: %s", errno, portname, strerror (errno));
         return FAILURE;
       }else
       {
         log_info ("device opened: %s", portname);
       }
     }else
     {
         log_info ("device opened: %s", portname);
     }
     set_interface_attribs(arduino_fd,B115200, 0);// set speed to 115,200 bps, 8n1 (no parity)
     set_blocking (arduino_fd, 0);                // set no blocking

     arduino_buf = malloc(sizeof(char) * arduino_buf_size);;
    /*
     * start the MOVI counters
     */

    int core;
    for (core = 0; core < num_cores; ++core) {
        before_time[core] = timer_get_time();
    }
    is_initialized = 1;
    return SUCCESS;
}


/* Initializes the MOVI plug-in units of metrics */
int
mf_movi_unit_init(void)
{
    int i;
    int ret = unit_file_check("movidius");
    if(ret != 0) {
        printf("unit file of movidius exists.\n");
        return FAILURE;
    }
    metric_units *unit = malloc(sizeof(metric_units));
    if (unit == NULL) {
        return FAILURE;
    }
    memset(unit, 0, sizeof(metric_units));
    for(i=0; i < MOVI_MAX_PRESET_EVENTS; i++) {
        unit->metric_name[i]=malloc(32*sizeof(char));
        unit->plugin_name[i]=malloc(32*sizeof(char));
        strcpy(unit->plugin_name[i], "mf_plugin_movidius");
        unit->unit[i]=malloc(4*sizeof(char));
        if(i <= 13) {
            strcpy(unit->unit[i], "mA");
        }
        else
            strcpy(unit->unit[i], "mV");   
    }
    strcpy(unit->metric_name[0], "VDDCR_A");
    strcpy(unit->metric_name[1], "VDDCV_A");
    strcpy(unit->metric_name[2], "DRAM_VDD1");
    strcpy(unit->metric_name[3], "MIPI_VDD_A");
    strcpy(unit->metric_name[4], "DRAM_VDD2");
    strcpy(unit->metric_name[5], "DRAM_VDDQ");
    strcpy(unit->metric_name[6], "DRAM_MVDDQ");
    strcpy(unit->metric_name[7], "DRAM_MVDDA");
    strcpy(unit->metric_name[8], "USB_VDD330");
    strcpy(unit->metric_name[9], "USB_VP_VDD");
    strcpy(unit->metric_name[10], "VDDIO");
    strcpy(unit->metric_name[11], "VDDIO_B");
    strcpy(unit->metric_name[12], "RESERVED");
    strcpy(unit->metric_name[13], "PLL_AVDD");
    strcpy(unit->metric_name[14], "VDDCV_V");
    strcpy(unit->metric_name[15], "MIPI_VDD_V");
    
    unit->num_metrics = MOVI_MAX_PRESET_EVENTS;
    publish_unit(unit);
    return SUCCESS;
}


/* Check if MOVI plugin is initialized */
static int
is_movi_initialized()
{
    return is_initialized;
}

/* Prepare the metrics */
static int
create_eventset_for(MOVI_Plugin *data, char **movi_events, size_t num_events)
{
    int retval, number_of_core, num_cores, ii;
    retval = SUCCESS;
    num_cores = 1;
    for (number_of_core = 0; number_of_core < num_cores; number_of_core++)
    {
        for(ii=0; ii < num_events; ii++)
        {
            data[number_of_core].events[ii]=(char*)malloc(256*sizeof(char));
            strcpy(data[number_of_core].events[ii], movi_events[ii]);
        }
        data[number_of_core].num_events = num_events;
    }
    return retval;
}

/* Prepare all the metrics */
static int
create_eventset_for_all(MOVI_Plugin *all_data)
{
    int retval, number_of_core, num_cores, ii;
    retval = SUCCESS;
    num_cores = 1;
    for (number_of_core = 0; number_of_core < num_cores; number_of_core++)
    {
        for(ii=0; ii < MOVI_MAX_PRESET_EVENTS; ii++)
        {
            all_data[number_of_core].events[ii]=(char*)malloc(256*sizeof(char));
            all_data[number_of_core].values[ii]=(double)ii;
        }
        sprintf(all_data[number_of_core].events[0],"VDDCR_A");
        sprintf(all_data[number_of_core].events[1],"VDDCV_A");
        sprintf(all_data[number_of_core].events[2],"DRAM_VDD1");
        sprintf(all_data[number_of_core].events[3],"MIPI_VDD_A");
        sprintf(all_data[number_of_core].events[4],"DRAM_VDD2");
        sprintf(all_data[number_of_core].events[5],"DRAM_VDDQ");
        sprintf(all_data[number_of_core].events[6],"DRAM_MVDDQ");
        sprintf(all_data[number_of_core].events[7],"DRAM_MVDDA");
        sprintf(all_data[number_of_core].events[8],"USB_VDD330");
        sprintf(all_data[number_of_core].events[9],"USB_VP_VDD");
        sprintf(all_data[number_of_core].events[10],"VDDIO");
        sprintf(all_data[number_of_core].events[11],"VDDIO_B ");
        sprintf(all_data[number_of_core].events[12],"RESERVED");
        sprintf(all_data[number_of_core].events[13],"PLL_AVDD");
        sprintf(all_data[number_of_core].events[14],"VDDCV_V");
        sprintf(all_data[number_of_core].events[15],"MIPI_VDD_V");
    }
    return retval;
}

/* Set the coefficients for various channels */
void set_channel_coef()
{
    channel_coef[0]=(1.0/20.0);
    channel_coef[1]=(1.0/0.6);
    channel_coef[2]=(1.0/20.0);
    channel_coef[3]=(1.0/20.0);
    channel_coef[4]=(1.0/10.0);
    channel_coef[5]=(1.0/20.0);
    channel_coef[6]=(1.0/2.0);
    channel_coef[7]=(1.0/20.0);
    channel_coef[8]=(1.0/20.0);
    channel_coef[9]=(1.0/20.0);
    channel_coef[10]=(1.0/10.0);
    channel_coef[11]=(1.0/20.0);
    channel_coef[12]=(0.0);
    channel_coef[13]=(1/20.0);
    channel_coef[14]=(1.0);
    channel_coef[15]=(1.0);
}

/* Samples given MOVI events */
int
mf_movi_sample(MOVI_Plugin *data)
{
    int core;
    size_t sample_size;
    for (core = 0; core < maximum_number_of_cores; core++) {
        /*
         * compute time interval used for sampling
         */
        after_time[core] = timer_get_time();

        /*
         * read, set, and reset counters
         */
         read_arduino(arduino_fd, arduino_buf, &sample_size, arduino_buf_size-1);
         convert(all_data,arduino_buf, sample_size, channel_coef);
         filter(all_data, data);

        /*
         * update time interval
         */
        before_time[core] = after_time[core];

        /*
         * reset counters to zero for next sample interval
         */

    }

    return SUCCESS;
}

/* String representation of sampled MOVI events */
char*
mf_movi_to_json(MOVI_Plugin *data)
{
    int core, event_idx;
    size_t num_events;
    //num_events = MOVI_MAX_PRESET_EVENTS; /* unused */
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(1024 * sizeof(char));
    strcpy(json, "\"type\":\"power\"");

    for (core = 0; core < maximum_number_of_cores; core++) {
        num_events = data[core].num_events;
        for (event_idx = 0; event_idx < num_events; event_idx++) {
            sprintf(metric, ",\"%s\":"FORMAT_SCIENTIFIC,  data[core].events[event_idx],data[core].values[event_idx]);
            strcat(json, metric);
        }
    }
    free(metric);
    log_info("%s\n",json);
    return json;
}

/* Stops sampling MOVI events */ 
void mf_movi_shutdown()
{
    close(arduino_fd);
}

/* Get current timestamp */
double timer_get_time(void)
{
    struct timespec time_now;
    double result_sec_now;

    clock_gettime(CLOCK_MONOTONIC,&time_now);
    result_sec_now=(time_now.tv_sec)+(time_now.tv_nsec)*1e-9;
    return result_sec_now;
}

/* Open serial interface to arduino */
int open_interfase(int* fd, const char* port_name)
{
    int fd_local = open (port_name, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd_local < 0)
    {
        log_info ("error %d opening %s: %s", errno, port_name, strerror (errno));
        return FAILURE;
    }
    *fd = fd_local;

    return SUCCESS;
}

/* Set speed and parity attributes of the interface */
int set_interface_attribs(int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                log_info ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                log_info ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

/* Set blocking or non-blocking attribute */
void set_blocking(int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                log_info ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                log_info ("error %d setting term attributes", errno);
}

/* Read the counters */
void read_arduino(int fd, char* arduino_output, size_t* length,  size_t max_length)
{
    *length = read (fd, arduino_output, max_length);
}

/* Convert the counters with respect to different coefficients */
void convert(MOVI_Plugin *data, char* arduino_output, size_t length,  double* ch_coef1)
{
    size_t current_pos;
    size_t current_numpos, num_length;
    char cr;
    int found, found2, channel,ii;
    char tmp[128];
    int first_measure[MOVI_MAX_PRESET_EVENTS];
    current_pos = 0;
    arduino_output[arduino_buf_size-1]=(char)0;
   // printf("STRING:%s\n",arduino_output);
    for(ii=0;ii<MOVI_MAX_PRESET_EVENTS;ii++)
    {
      data[0].values[ii] = 0.0;
      first_measure[ii] = 0;
    }
    while(current_pos<length)
    {
        found=0;
        found2=0;
        channel = 0;
        while(current_pos<length && found == 0)
        {
            cr=arduino_output[current_pos];
            switch( cr)
            {
                case '#':
                {
                  current_pos=current_pos+1;
                  cr=arduino_output[current_pos];
                  while(current_pos<length  && found == 0)
                  {
                     cr=arduino_output[current_pos];
                     switch( cr)
                     {
                        case ';':
                        {
                            current_pos = current_pos + 1;
                            found = 1;
                            break;
                        }
                        default:
                            current_pos = current_pos + 1;
                            break;
                      }
                  }
                  break;
                }
                default:
                   current_pos = current_pos + 1;
                   break;
             }
         }
         found=0;
         current_numpos=0;
         channel = 0;
         while(current_pos<length && found2 == 0)
         {
             cr=arduino_output[current_pos];
             switch( cr)
             {
                 case ';':
                 {
                     current_pos = current_pos + 1;
                     num_length = current_numpos;
                     current_numpos = 0;
                     found = 1;
                     tmp[num_length]=(char)0;
                     break;
                 }
                 case '#':
                 {
                      found2 = 1;
                      current_pos = current_pos + 1;
                      num_length = current_numpos+1;
                      current_numpos = 0;
                      found = 1;
                      tmp[num_length]=(char)0;
                      break;
                 }
                 default:
                 {
                   tmp[current_numpos]=arduino_output[current_pos];
                   current_pos = current_pos + 1;
                   current_numpos = current_numpos +1;
                   break;
                 }
             }
             if(found == 1)
             {
                 double number = (double)atoi(tmp);
                 if(first_measure[channel]==0)
                 {
                   data[0].values[channel] = ((number)/1.0e03)*ch_coef1[channel];
                   first_measure[channel]++;
                 }else
                 {
                   data[0].values[channel] += ((number)/1.0e03)*ch_coef1[channel];
                   data[0].values[channel] = data[0].values[channel] / 2.0;
                   first_measure[channel]++;
                 }
                 current_numpos = 0;
                 found=0;
                 channel++;
                 if (channel > MOVI_MAX_PRESET_EVENTS-1) found2=1;
             }
         }
     }
}

/* Filter the readed values with respected to the required metrics */
void filter(MOVI_Plugin *all_data, MOVI_Plugin *data)
{
    int ii, jj;
    for(ii = 0; ii < data[0].num_events; ii++) {
        for (jj = 0; jj < MOVI_MAX_PRESET_EVENTS; jj++) {
            if(strcmp(data[0].events[ii], all_data[0].events[jj]) != 0) {
                continue;
            }
            data[0].values[ii] = all_data[0].values[jj];
        }
    }
}
