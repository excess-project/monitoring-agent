/*
 * Copyright 2015 Anders Gidenstam, Chalmers University of Technology, Sweden.
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
#include <string.h>

#include <nvml.h>

typedef nvmlDevice_t** handle_t;

static handle_t mf_nvml_init();
/* Functions to get and append current device state information.
 * See http://docs.nvidia.com/deploy/nvml-api/group__nvmlDeviceQueries.html
 * for more information.
 */
static char *mf_nvml_append_perf_state(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_power_usage(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_power_limit(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_utilization(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_encoder_utilization(handle_t devices,
                                                char *buf, char *end);
static char *mf_nvml_append_decoder_utilization(handle_t devices,
                                                char *buf, char *end);
static char *mf_nvml_append_clock_freqs(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_clock_throttle_reasons(handle_t devices,
                                                   char *buf, char *end);
static char *mf_nvml_append_mem(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_mem_BAR1(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_mem_L1_ECC_errors(handle_t devices,
                                              char *buf, char *end);
static char *mf_nvml_append_mem_device_ECC_errors(handle_t devices,
                                                  char *buf, char *end);
static char *mf_nvml_append_mem_register_ECC_errors(handle_t devices,
                                                    char *buf, char *end);
static char *mf_nvml_append_mem_texture_ECC_errors(handle_t devices,
                                                   char *buf, char *end);
static char *mf_nvml_append_PCIe_throughput(handle_t devices,
                                            char *buf, char *end);
static char *mf_nvml_append_temperature(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_fan_speed(handle_t devices, char *buf, char *end);
static char *mf_nvml_append_throttled_time(handle_t devices,
                                           char *buf, char *end);

/* Macro APPEND_ON_SUCCESS
 *   Appends a successfully read value, ignores unsupported and reports other
 *   errors.
 */
#define APPEND_ON_SUCCESS(ret, buf, end, format, id, value, func)   \
  {                                                                 \
    switch (ret) {                                                  \
    case NVML_SUCCESS:                                              \
      buf = append_formatted(buf, end, format, id, value);          \
      break;                                                        \
    case NVML_ERROR_NOT_SUPPORTED:                                  \
      break;                                                        \
    default:                                                        \
      fprintf(stderr,                                               \
              "mf_nvml_append(): %s failed with return code %d.\n", \
              func, ret);                                           \
      break;                                                        \
    }                                                               \
  }

static handle_t mf_nvml_init()
{
  nvmlReturn_t ret = nvmlInit();
  switch (ret) {
  case NVML_SUCCESS:
    break;
  default:
    fprintf(stderr,
            "mf_nvml_init(): Failed to initialize the nvml library with "
            "return code %d.\n",
            ret);
    return NULL;
    break;
  }

  unsigned int device_count = 0;
  ret = nvmlDeviceGetCount(&device_count);
  switch (ret) {
  case NVML_SUCCESS:
    break;
  default:
    fprintf(stderr,
            "mf_nvml_init(): nvmlDeviceGetCount() failed with "
            "return code %d.\n",
            ret);
    return NULL;
    break;
  }

  nvmlDevice_t** devices = calloc(device_count+1, sizeof(nvmlDevice_t*));
  for (int i = 0; i < device_count; i++) {
    devices[i] = malloc(sizeof(nvmlDevice_t));
    ret = nvmlDeviceGetHandleByIndex(i, devices[i]);
    switch (ret) {
    case NVML_SUCCESS:
      break;
    default:
      fprintf(stderr,
              "mf_nvml_init(): nvmlDeviceGetHandleByIndex() failed with "
              "return code %d.\n",
              ret);
      break;
    }
  }

  return devices;
}

static char *mf_nvml_append_perf_state(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    nvmlPstates_t state;
    nvmlReturn_t ret = nvmlDeviceGetPerformanceState(*devices[i], &state);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:performance_state\":\"%d\"",
                      i, state,
                      "nvmlDeviceGetPerformanceState()");
  }
  return buf;
}

static char *mf_nvml_append_power_usage(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int power_mW;
    nvmlReturn_t ret = nvmlDeviceGetPowerUsage(*devices[i], &power_mW);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:power\":\"%f\"",
                      i, power_mW/1000.0,
                      "nvmlDeviceGetPowerUsage()");
  }
  return buf;
}

static char *mf_nvml_append_power_limit(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int power_mW;
    nvmlReturn_t ret = nvmlDeviceGetEnforcedPowerLimit(*devices[i], &power_mW);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:power_limit\":\"%f\"",
                      i, power_mW/1000.0,
                      "nvmlDeviceGetEnforcedPowerLimit()");
  }
  return buf;
}

static char *mf_nvml_append_utilization(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    nvmlUtilization_t utilization;
    nvmlReturn_t ret = nvmlDeviceGetUtilizationRates(*devices[i], &utilization);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:GPU_utilization\":\"%f\"",
                      i, utilization.gpu/100.0,
                      "nvmlDeviceGetUtilizationRates()");
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:MEM_utilization\":\"%f\"",
                      i, utilization.memory/100.0,
                      "nvmlDeviceGetUtilizationRates()");
  }
  return buf;
}

static char *mf_nvml_append_encoder_utilization(handle_t devices,
                                                char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int utilization;
    unsigned int sampling_period_us;
    nvmlReturn_t ret =
      nvmlDeviceGetEncoderUtilization(*devices[i],
                                      &utilization, &sampling_period_us);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:encoder_utilization\":\"%f\"",
                      i, utilization/100.0,
                      "nvmlDeviceGetEncoderUtilization()");
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:encoder_sampling_period\":\"%f\"",
                      i, sampling_period_us/1000000.0,
                      "nvmlDeviceGetEncoderUtilization()");
  }
  return buf;
}

static char *mf_nvml_append_decoder_utilization(handle_t devices,
                                                char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int utilization;
    unsigned int sampling_period_us;
    nvmlReturn_t ret =
      nvmlDeviceGetDecoderUtilization(*devices[i],
                                      &utilization, &sampling_period_us);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:decoder_utilization\":\"%f\"",
                      i, utilization/100.0,
                      "nvmlDeviceGetDecoderUtilization()");
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:decoder_sampling_period\":\"%f\"",
                      i, sampling_period_us/1000000.0,
                      "nvmlDeviceGetDecoderUtilization()");
  }
  return buf;
}

static char *mf_nvml_append_clock_freqs(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int clockMHz;
    nvmlReturn_t ret =
      nvmlDeviceGetClockInfo(*devices[i], NVML_CLOCK_GRAPHICS, &clockMHz);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:GRAPHICS_clock\":\"%.1lf\"",
                      i, clockMHz*1e6,
                      "nvmlDeviceGetClockInfo()");

    ret = nvmlDeviceGetClockInfo(*devices[i], NVML_CLOCK_SM, &clockMHz);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:SM_clock\":\"%.1lf\"",
                      i, clockMHz*1e6,
                      "nvmlDeviceGetClockInfo()");

    ret = nvmlDeviceGetClockInfo(*devices[i], NVML_CLOCK_MEM, &clockMHz);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:MEM_clock\":\"%.1lf\"",
                      i, clockMHz*1e6,
                      "nvmlDeviceGetClockInfo()");
  }
  return buf;
}

static char *mf_nvml_append_clock_throttle_reasons(handle_t devices,
                                                   char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned long long clocksThrottleReasons;
    nvmlReturn_t ret =
      nvmlDeviceGetCurrentClocksThrottleReasons(*devices[i],
                                                &clocksThrottleReasons);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:clocks_throttle_reasons\":\"%lld\"",
                      i, clocksThrottleReasons,
                      "nvmlDeviceGetCurrentClocksThrottleReasons()");
  }
  return buf;
}

static char *mf_nvml_append_mem(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    nvmlMemory_t mem_info;
    nvmlReturn_t ret = nvmlDeviceGetMemoryInfo(*devices[i], &mem_info);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:MEM_used\":\"%ld\"",
                      i, mem_info.used,
                      "nvmlDeviceGetMemoryInfo()");
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:MEM_free\":\"%ld\"",
                      i, mem_info.free,
                      "nvmlDeviceGetMemoryInfo()");
  }
  return buf;
}

static char *mf_nvml_append_mem_BAR1(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    nvmlBAR1Memory_t mem_info;
    nvmlReturn_t ret = nvmlDeviceGetBAR1MemoryInfo(*devices[i], &mem_info);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:BAR1_MEM_used\":\"%ld\"",
                      i, mem_info.bar1Used,
                      "nvmlDeviceGetBAR1MemoryInfo()");
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:BAR1_MEM_free\":\"%ld\"",
                      i, mem_info.bar1Free,
                      "nvmlDeviceGetBAR1MemoryInfo()");
  }
  return buf;
}

static char *mf_nvml_append_mem_L1_ECC_errors(handle_t devices,
                                              char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned long long lifetime_errors;
    nvmlReturn_t ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_CORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_L1_CACHE,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_corrected_errors_L1_cache\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
    ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_UNCORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_L1_CACHE,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_uncorrected_errors_L1_cache\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
  }
  return buf;
}

static char *mf_nvml_append_mem_L2_ECC_errors(handle_t devices,
                                              char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned long long lifetime_errors;
    nvmlReturn_t ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_CORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_L2_CACHE,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_corrected_errors_L2_cache\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
    ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_UNCORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_L2_CACHE,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_uncorrected_errors_L2_cache\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
  }
  return buf;
}

static char *mf_nvml_append_mem_device_ECC_errors(handle_t devices,
                                                  char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned long long lifetime_errors;
    nvmlReturn_t ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_CORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_DEVICE_MEMORY,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_corrected_errors_device_mem\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
    ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_UNCORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_DEVICE_MEMORY,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_uncorrected_errors_device_mem\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
  }
  return buf;
}

static char *mf_nvml_append_mem_register_ECC_errors(handle_t devices,
                                                    char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned long long lifetime_errors;
    nvmlReturn_t ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_CORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_REGISTER_FILE,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_corrected_errors_register_file\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
    ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_UNCORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_REGISTER_FILE,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_uncorrected_errors_register_file\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
  }
  return buf;
}

static char *mf_nvml_append_mem_texture_ECC_errors(handle_t devices,
                                                   char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned long long lifetime_errors;
    nvmlReturn_t ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_CORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_TEXTURE_MEMORY,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_corrected_errors_texture_mem\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
    ret =
      nvmlDeviceGetMemoryErrorCounter(*devices[i],
                                      NVML_MEMORY_ERROR_TYPE_UNCORRECTED,
                                      NVML_AGGREGATE_ECC,
                                      NVML_MEMORY_LOCATION_TEXTURE_MEMORY,
                                      &lifetime_errors);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:ECC_uncorrected_errors_texture_mem\":\"%ld\"",
                      i, lifetime_errors,
                      "nvmlDeviceGetMemoryErrorCounter()");
  }
  return buf;
}

static char *mf_nvml_append_PCIe_throughput(handle_t devices,
                                            char *buf, char *end)
{
#ifdef nvmlDeviceGetPcieThroughput
  for (int i = 0; devices[i]; i++) {
    unsigned int bytes_per_20ms;
    // Returns #bytes during a 20msec interval. Transform to bytes/sec
    nvmlReturn_t ret = nvmlDeviceGetPcieThroughput(*devices[i],
                                                   NVML_PCIE_UTIL_TX_BYTES,
                                                   &bytes_per_20ms);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:PCIe_tx_throughput\":\"%.1lf\"",
                      i, bytes_per_20ms/0.020,
                      "nvmlDeviceGetPcieThroughput()");
    ret = nvmlDeviceGetPcieThroughput(*devices[i],
                                      NVML_PCIE_UTIL_RX_BYTES,
                                      &bytes_per_20ms);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:PCIe_rx_throughput\":\"%.1lf\"",
                      i, bytes_per_20ms/0.020,
                      "nvmlDeviceGetPcieThroughput()");
  }
#endif
  return buf;
}

static char *mf_nvml_append_temperature(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int tempC;
    nvmlReturn_t ret = nvmlDeviceGetTemperature(*devices[i],
                                                NVML_TEMPERATURE_GPU, &tempC);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:temperature\":\"%d\"",
                      i, tempC,
                      "nvmlDeviceGetTemperature()");
  }
  return buf;
}

static char *mf_nvml_append_fan_speed(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int fan_speed;
    nvmlReturn_t ret = nvmlDeviceGetFanSpeed(*devices[i], &fan_speed);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:fan_speed\":\"%f\"",
                      i, fan_speed/100.0,
                      "nvmlDeviceGetFanSpeed()");
  }
  return buf;
}

static char *mf_nvml_append_throttled_time(handle_t devices,
                                           char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    nvmlViolationTime_t time;
    nvmlReturn_t ret = nvmlDeviceGetViolationStatus(*devices[i],
                                                    NVML_PERF_POLICY_POWER,
                                                    &time);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:time_power_capped\":\"%e\"",
                      i, time.violationTime/1.0e-9,
                      "nvmlDeviceGetViolationStatus()");
    ret = nvmlDeviceGetViolationStatus(*devices[i],
                                       NVML_PERF_POLICY_THERMAL,
                                       &time);
    APPEND_ON_SUCCESS(ret, buf, end,
                      ",\"GPU%d:time_thermal_capped\":\"%e\"",
                      i, time.violationTime/1.0e-9,
                      "nvmlDeviceGetViolationStatus()");
  }
  return buf;
}

