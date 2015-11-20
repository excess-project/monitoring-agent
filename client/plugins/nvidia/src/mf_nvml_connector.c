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
static char *mf_nvml_append_power(handle_t device, char *buf, char *end);
static char *mf_nvml_append_utilization(handle_t device, char *buf, char *end);
static char *mf_nvml_append_temperature(handle_t device, char *buf, char *end);
static char *mf_nvml_append_fan_speed(handle_t device, char *buf, char *end);

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

static char *mf_nvml_append_power(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int power_mW;
    nvmlReturn_t ret = nvmlDeviceGetPowerUsage(*devices[i], &power_mW);
    switch (ret) {
    case NVML_SUCCESS:
      buf = append_formatted(buf, end,
                             ",\"GPU%d:power\":\"%f\"",
                             i, power_mW/1000.0);
      break;
    case NVML_ERROR_NOT_SUPPORTED:
      break;
    default:
      fprintf(stderr,
              "mf_nvml_append(): nvmlDeviceGetPowerUsage() failed "
              "with return code %d.\n",
              ret);
      break;
    }
  }
  return buf;
}

static char *mf_nvml_append_utilization(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    nvmlUtilization_t utilization;
    nvmlReturn_t ret = nvmlDeviceGetUtilizationRates(*devices[i], &utilization);
    switch (ret) {
    case NVML_SUCCESS:
      buf = append_formatted(buf, end,
                             ",\"GPU%d:GPUutilization\":\"%f\"",
                             i, utilization.gpu/100.0);
      buf = append_formatted(buf, end,
                             ",\"GPU%d:MEMutilization\":\"%f\"",
                             i, utilization.memory/100.0);
      break;
    case NVML_ERROR_NOT_SUPPORTED:
      break;
    default:
      fprintf(stderr,
              "mf_nvml_append(): nvmlDeviceGetUtilizationRates() failed "
              "with return code %d.\n",
              ret);
      break;
    }
  }
  return buf;
}

static char *mf_nvml_append_temperature(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int tempC;
    nvmlReturn_t ret = nvmlDeviceGetTemperature(*devices[i],
                                                NVML_TEMPERATURE_GPU, &tempC);
    switch (ret) {
    case NVML_SUCCESS:
      buf = append_formatted(buf, end,
                             ",\"GPU%d:temperature\":\"%d\"",
                             i, tempC);
      break;
    case NVML_ERROR_NOT_SUPPORTED:
      break;
    default:
      fprintf(stderr,
              "mf_nvml_append(): nvmlDeviceGetTemperature() failed "
              "with return code %d.\n",
              ret);
      break;
    }
  }
  return buf;
}

static char *mf_nvml_append_fan_speed(handle_t devices, char *buf, char *end)
{
  for (int i = 0; devices[i]; i++) {
    unsigned int fan_speed;
    nvmlReturn_t ret = nvmlDeviceGetFanSpeed(*devices[i], &fan_speed);
    switch (ret) {
    case NVML_SUCCESS:
      buf = append_formatted(buf, end,
                             ",\"GPU%d:fan_speed\":\"%f\"",
                             i, fan_speed/100.0);
      break;
    case NVML_ERROR_NOT_SUPPORTED:
      break;
    default:
      fprintf(stderr,
              "mf_nvml_append(): nvmlDeviceGetFanSpeed() failed "
              "with return code %d.\n",
              ret);
      break;
    }
  }
  return buf;
}
