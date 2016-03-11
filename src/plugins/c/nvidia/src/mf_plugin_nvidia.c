/*
 * Copyright 2015 Anders Gidenstam, Chalmers University of Technology.
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

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "util.h"
#include "plugin_manager.h"
#include "excess_main.h"

/* Internal code. */
static char *append(char *buf, char *end, const char *msg);
static char *append_formatted(char *buf, char *end, const char *format, ...);
static char *create_JSON_msg();
static int is_enabled(const char *key);

#include "mf_nvidia_connector.c"

/* Plugin global data. */
static mfp_data *nvidia_conf_data;
static handle_t nvidia_handle;
static unsigned int max_msg_length = 128;

static metric mf_plugin_nvidia_hook()
{
  //fprintf(stderr, "init_mf_plugin_nvidia_hook() called!\n");
  if (running) {
    metric resMetric = malloc(sizeof(metric_t));

    clock_gettime(CLOCK_REALTIME, &resMetric->timestamp);

    /* Fill in the JSON data. */
    resMetric->msg = create_JSON_msg();

    /* Debug printout. */
//    fprintf(stderr,
//            "{\"Timestamp\":\"%Lf\"%s}\n",
//            resMetric->timestamp.tv_sec +
//            1.0e-9 * (long double)resMetric->timestamp.tv_nsec,
//            resMetric->msg);

    return resMetric;
  } else {
    return NULL ;
  }
}

extern int init_mf_plugin_nvidia(PluginManager *pm)
{
  //fprintf(stderr, "init_mf_plugin_nvidia() called!\n");
  PluginManager_register_hook(pm, "mf_plugin_nvidia", mf_plugin_nvidia_hook);

  nvidia_conf_data = malloc(sizeof(mfp_data));
  mfp_get_data_filtered_by_value("mf_plugin_nvidia", nvidia_conf_data, "on");

  nvidia_handle = mf_nvml_init();

  //fprintf(stderr, "init_mf_plugin_nvidia() initialized!\n");

  return 1;
}

static char *append(char *buf, char *end, const char *msg)
{
  if (buf + strlen(msg) + 1 < end) {
    strcpy(buf, msg);
    return buf + strlen(msg);
  } else {
    return end;
  }
}

static char *append_formatted(char *buf, char *end, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buf, end-buf, format, args);
  va_end(args);

  if (buf+len < end) {
    return buf + len;
  } else {
    *(end-1) = '\0';
    return end;
  }
}

static char *create_JSON_msg()
{
  char *msg = malloc(max_msg_length * sizeof(char));
  char *buf = msg;
  char *end = buf + max_msg_length;

  buf = append(buf, end, "\"type\":\"GPU\"");
  if (is_enabled("performance_state")) {
    buf = mf_nvml_append_perf_state(nvidia_handle, buf, end);
  }
  if (is_enabled("power_usage")) {
    buf = mf_nvml_append_power_usage(nvidia_handle, buf, end);
  }
  if (is_enabled("power_limit")) {
    buf = mf_nvml_append_power_limit(nvidia_handle, buf, end);
  }
  if (is_enabled("utilization")) {
    buf = mf_nvml_append_utilization(nvidia_handle, buf, end);
  }
  if (is_enabled("encoder_utilization")) {
    buf = mf_nvml_append_encoder_utilization(nvidia_handle, buf, end);
  }
  if (is_enabled("decoder_utilization")) {
    buf = mf_nvml_append_decoder_utilization(nvidia_handle, buf, end);
  }
  if (is_enabled("clock_frequencies")) {
    buf = mf_nvml_append_clock_freqs(nvidia_handle, buf, end);
  }
  if (is_enabled("clock_throttle_reasons")) {
    buf = mf_nvml_append_clock_throttle_reasons(nvidia_handle, buf, end);
  }
  if (is_enabled("memory")) {
    buf = mf_nvml_append_mem(nvidia_handle, buf, end);
  }
  if (is_enabled("memory_BAR1")) {
    buf = mf_nvml_append_mem_BAR1(nvidia_handle, buf, end);
  }
  if (is_enabled("L1_cache_ECC_errors")) {
    buf = mf_nvml_append_mem_L1_ECC_errors(nvidia_handle, buf, end);
  }
  if (is_enabled("L2_cache_ECC_errors")) {
    buf = mf_nvml_append_mem_L2_ECC_errors(nvidia_handle, buf, end);
  }
  if (is_enabled("memory_ECC_errors")) {
    buf = mf_nvml_append_mem_device_ECC_errors(nvidia_handle, buf, end);
  }
  if (is_enabled("register_file_ECC_errors")) {
    buf = mf_nvml_append_mem_register_ECC_errors(nvidia_handle, buf, end);
  }
  if (is_enabled("texture_memory_ECC_errors")) {
    buf = mf_nvml_append_mem_texture_ECC_errors(nvidia_handle, buf, end);
  }
  if (is_enabled("PCIe_throughput")) {
    buf = mf_nvml_append_PCIe_throughput(nvidia_handle, buf, end);
  }
  if (is_enabled("temperature")) {
    buf = mf_nvml_append_temperature(nvidia_handle, buf, end);
  }
  if (is_enabled("fan_speed")) {
    buf = mf_nvml_append_fan_speed(nvidia_handle, buf, end);
  }
  if (is_enabled("time_throttled")) {
    buf = mf_nvml_append_throttled_time(nvidia_handle, buf, end);
  }
  if (buf == end) {
    fprintf(stderr, "mf_plugin_nvidia(): message length overflow!\n");
    free(msg);
    max_msg_length *= 2;
    msg = create_JSON_msg();
  }
  return msg;
}

static int is_enabled(const char *key)
{
  char value[20]={'\0'};
  mfp_get_value("mf_plugin_nvidia", key, value);
  //char *value = mfp_get_value("mf_plugin_nvidia", key);
  return value != NULL && !strcmp(value, "on");
}
