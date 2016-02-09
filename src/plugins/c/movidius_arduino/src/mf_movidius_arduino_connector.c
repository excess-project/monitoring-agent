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
/* monitoring-related includes */
#include "mf_debug.h"
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
/*******************************************************************************
 * FORWARD DECLARATIONS
 ******************************************************************************/

static int is_movi_initialized();
static int create_eventset_for();






/*******************************************************************************
 * mf_papi_init
 ******************************************************************************/

int
mf_movi_init(
    MOVI_Plugin *data,
    size_t num_events,
    size_t num_cores)
{
    if (is_movi_initialized()) {
        return SUCCESS;
    }
    maximum_number_of_cores = num_cores; 
    channel_coef = malloc(num_events*sizeof(double));
     /* creates EventSets for each individual core
     */
    create_eventset_for(data,channel_coef,channel_coef);
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
        log_info ("error %d by opening %s: %s", errno, portname, strerror (errno));
        return FAILURE;
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

/*******************************************************************************
 * is_movi_initialized
 ******************************************************************************/

static int
is_movi_initialized()
{
    return is_initialized;
}

/*******************************************************************************
 * create_eventset_for
 ******************************************************************************/

static int
create_eventset_for(MOVI_Plugin *data, double* ch_coef1, int num_cores)
{
    int retval;
    int number_of_core;
    int ii;
    
    retval = SUCCESS;
    
    for (number_of_core = 0; number_of_core < num_cores; ++number_of_core) 
    {
        for(ii=0;ii<MOVI_MAX_PRESET_EVENTS;ii++)
        {
          data[number_of_core].events[ii]=(char*)malloc(256*sizeof(char));
          data[number_of_core].values[ii]=(double)ii;
        }
        sprintf(data[number_of_core].events[0],"VDDCR_A");
        sprintf(data[number_of_core].events[1],"VDDCV_A");
        sprintf(data[number_of_core].events[2],"DRAM_VDD1");
        sprintf(data[number_of_core].events[3],"MIPI_VDD");
        sprintf(data[number_of_core].events[4],"DRAM_VDD2");
        sprintf(data[number_of_core].events[5],"DRAM_VDDQ");
        sprintf(data[number_of_core].events[6],"DRAM_MVDDQ");
        sprintf(data[number_of_core].events[7],"DRAM_MVDDA");
        sprintf(data[number_of_core].events[8],"USB_VDD330");
        sprintf(data[number_of_core].events[9],"USB_VP_VDD");
        sprintf(data[number_of_core].events[10],"VDDIO");
        sprintf(data[number_of_core].events[11],"VDDIO_B ");
        sprintf(data[number_of_core].events[12],"RESERVED");
        sprintf(data[number_of_core].events[13],"PLL_AVDD");
        sprintf(data[number_of_core].events[14],"VDDCV_A");
        sprintf(data[number_of_core].events[15],"MIPI_VDD");
        ch_coef1[0]=(1.0/20.0);
        ch_coef1[1]=(1.0/0.6);
        ch_coef1[2]=(1.0/20.0);
        ch_coef1[3]=(1.0/20.0);
        ch_coef1[4]=(1.0/10.0);
        ch_coef1[5]=(1.0/20.0);
        ch_coef1[6]=(1.0/2.0);
        ch_coef1[7]=(1.0/20.0);
        ch_coef1[8]=(1.0/20.0);
        ch_coef1[9]=(1.0/20.0);
        ch_coef1[10]=(1.0/10.0);
        ch_coef1[11]=(1.0/20.0);
        ch_coef1[12]=(0.0);
        ch_coef1[13]=(1/20.0);
        ch_coef1[14]=(1.0);
        ch_coef1[15]=(1.0);
        data[number_of_core].num_events = MOVI_MAX_PRESET_EVENTS;
    }

    return retval;
}

/*******************************************************************************
 * mf_papi_sample
 ******************************************************************************/

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
         convert(data,arduino_buf, sample_size, channel_coef);

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

/*******************************************************************************
 * mf_papi_to_json
 ******************************************************************************/

char*
mf_movi_to_json(MOVI_Plugin *data)
{
    int core, event_idx;
    size_t num_events;
    num_events = MOVI_MAX_PRESET_EVENTS;
    char *metric = malloc(512 * sizeof(char));
    char *json = malloc(4096 * sizeof(char));
    strcpy(json, ",\"type\":\"power\"");

    for (core = 0; core < maximum_number_of_cores; core++) {
        num_events = data[core].num_events;
        for (event_idx = 0; event_idx < MOVI_MAX_PRESET_EVENTS; event_idx++) {
            sprintf(metric, ",\"%s\":"FORMAT_SCIENTIFIC,  data[core].events[event_idx],data[core].values[event_idx]*channel_coef[event_idx]);
            strcat(json, metric);
        }
    }
    free(metric);
  //  printf("%s\n",json);
    return json;
}

/*******************************************************************************
 * mf_papi_shutdown
 ******************************************************************************/

void mf_movi_shutdown()
{
  close(arduino_fd);
}

double timer_get_time(void)
{
    struct timespec time_now;
    double result_sec_now;

    clock_gettime(CLOCK_MONOTONIC,&time_now);
    result_sec_now=(time_now.tv_sec)+(time_now.tv_nsec)*1e-9;
    return result_sec_now;
}

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

int set_interface_attribs (int fd, int speed, int parity)
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

void set_blocking (int fd, int should_block)
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

void read_arduino(int fd, char* arduino_output, size_t* length,  size_t max_length)
{
    *length = read (fd, arduino_output, max_length); 
}

void convert(MOVI_Plugin *data, char* arduino_output, size_t length,  double* ch_coef1 )
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
      //   printf("STRING TO PARSE:%s\n",arduino_output[current_pos]);
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
                    // printf("FIND:\";\" current_pos:%d; num_length: %d; SUBSTRING:%s\n",current_pos, num_length, tmp);
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
                    //  printf("FIND:\"#\" current_pos:%d; num_length: %d; SUBSTRING:%s\n",current_pos, num_length, tmp);
                      break;
                 }
                 default:
                 {
                   tmp[current_numpos]=arduino_output[current_pos];
                   current_pos = current_pos + 1;
                   current_numpos = current_numpos +1;
                 //   printf("FIND:\"[0-9]\" current_pos:%d; current_numpos: %d; NUMBER:%c\n",current_pos, current_numpos, tmp[current_numpos-1]);
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
             //    printf("CHANNEL:%d; STRING:%s; COVERT: %f; RESULT: %f; current_pos: %d; length: %d\n",channel, tmp, number, data[0].values[channel],current_pos,length);
                 channel++;
                 if (channel > MOVI_MAX_PRESET_EVENTS-1) found2=1;
             }
         }
     }
          
}
