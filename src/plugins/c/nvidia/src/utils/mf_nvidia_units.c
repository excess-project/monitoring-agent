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

/** @file mf_nvidia_unit.c
 *   @brief initialize nvidia units, send to mf_server then
 *
 *   @author Fangli Pi
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> /* malloc, exit, free, ... */
#include <string.h>
#include <nvml.h>
#include <mf_parser.h>
#include "mf_types.h"
#include "publisher.h"


int mf_nvml_unit_init(int dev_count);

/* A client to initialize nvidia plugin units of metrics */
int main(void)
{
	printf("[mf_nvidia_units] starting...\n");

	nvmlReturn_t ret = nvmlInit();
	if(ret != NVML_SUCCESS) {
		printf("[mf_nvidia_units] nvml init failed.\n");
		return -1;
	}
	
	unsigned int device_count = 0;
	ret = nvmlDeviceGetCount(&device_count);
	if(ret != NVML_SUCCESS) {
		printf("[mf_nvidia_units] nvml get device count failed.\n");
		return -1;
	}

	int r = mf_nvml_unit_init(device_count);
	if(r != 1) {
		printf("[mf_nvidia_units] nvml units init failed.\n");
		return -1;
	}
	return 1;
}

/* Initialize units of metrics */
int mf_nvml_unit_init(int dev_count)
{
  int i, conf_i, dev;
  metric_units *unit = malloc(sizeof(metric_units));
  if(unit== NULL) {
    return 0;
  }
  memset(unit, 0, sizeof(metric_units));

  mfp_data *conf_data =  malloc(sizeof(mfp_data));
  mfp_get_data("mf_plugin_nvidia", conf_data);

  i=0;
  for(conf_i = 0; conf_i < conf_data->size; conf_i++) {
    if(strcmp(conf_data->keys[conf_i], "power_usage") == 0) {
      for (dev=0; dev < dev_count; dev++) {
        unit->metric_name[i] =malloc(32 * sizeof(char));
        unit->plugin_name[i] =malloc(32 * sizeof(char));
        unit->unit[i] =malloc(6 * sizeof(char));
        sprintf(unit->metric_name[i], "GPU%d:power", dev);
        strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
        strcpy(unit->unit[i], "Watt");
        i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "power_limit") == 0) {
        for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:power_limit", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "Watt");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "utilization") == 0) {
        for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:GPU_utilization", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "%");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:MEM_utilization", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "%");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "encoder_utilization") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:encoder_utilization", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "%");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:encoder_sampling_period", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "s");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "decoder_utilization") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:decoder_utilization", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "%");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:decoder_sampling_period", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "s");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "clock_frequencies") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:GRAPHICS_clock", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "Hz");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:SM_clock", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "Hz");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:MEM_clock", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "Hz");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "memory") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:MEM_used", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "bytes");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:MEM_free", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "bytes");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "memory_BAR1") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:BAR1_MEM_used", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "bytes");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(6 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:BAR1_MEM_free", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "bytes");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "PCIe_throughput") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(10 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:PCIe_tx_throughput", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "bytes/s");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(10 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:PCIe_rx_throughput", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "bytes/s");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "temperature") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(4 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:temperature", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "°c");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "fan_speed") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(4 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:fan_speed", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "%");
          i++;
      }
    }

    if(strcmp(conf_data->keys[conf_i], "time_throttled") == 0) {
      for (dev=0; dev < dev_count; dev++) {
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(10 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:time_power_capped", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "s");
          i++;
          unit->metric_name[i] =malloc(32 * sizeof(char));
          unit->plugin_name[i] =malloc(32 * sizeof(char));
          unit->unit[i] =malloc(10 * sizeof(char));
          sprintf(unit->metric_name[i], "GPU%d:time_thermal_capped", dev);
          strcpy(unit->plugin_name[i], "mf_plugin_nvidia");
          strcpy(unit->unit[i], "s");
          i++;
      }
    }
  }
  unit->num_metrics = i;
  publish_unit(unit);
  return 1;
}
