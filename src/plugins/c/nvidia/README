# EXCESS ATOM Monitoring Framework: NVIDIA Plug-in

We currently do not provide a client test-suite for the NVIDIA plug-in. The
plug-in is configurable through the global **mf_config.ini**.


## List of supported events

It follows a list of available events, that can be set in the **mf_config.ini**.


| Available Metrics         | Mapped Metric Names                  | NVML Native Events       |
|---------------------------|--------------------------------------|--------------------------|
| performance_state         | performance_state                    |                          |
|---------------------------|--------------------------------------|--------------------------|
| power_usage               | power                                |                          |
|---------------------------|--------------------------------------|--------------------------|
| power_limit               | power_limit                          |                          |
|---------------------------|--------------------------------------|--------------------------|
| utilization               | GPU_utilization                      |                          |
|                           | MEM_utilization                      |                          |
|---------------------------|--------------------------------------|--------------------------|
| encoder_utilization       | encoder_utilization                  |                          |
|                           | encoder_sampling_period              |                          |
|---------------------------|--------------------------------------|--------------------------|
| decoder_utilization       | decoder_utilization                  |                          |
|                           | decoder_sampling_period              |                          |
|---------------------------|--------------------------------------|--------------------------|
| clock_frequencies         | GRAPHICS_clock                       | NVML_CLOCK_GRAPHICS      |
|                           | SM_clock                             | NVML_CLOCK_SM            |
|                           | MEM_clock                            | NVML_CLOCK_MEM           |
|---------------------------|--------------------------------------|--------------------------|
| clock_throttle_reasons    | clocks_throttle_reasons              |                          |
|---------------------------|--------------------------------------|--------------------------|
| memory                    | MEM_used                             |                          |
|                           | MEM_free                             |                          |
|---------------------------|--------------------------------------|--------------------------|
| memory_BAR1               | BAR1_MEM_used                        |                          |
|                           | BAR1_MEM_free                        |                          |
|---------------------------|--------------------------------------|--------------------------|
| L1_cache_ECC_errors       | ECC_corrected_errors_L1_cache        |                          |
|                           | ECC_uncorrected_errors_L1_cache      |                          |
|---------------------------|--------------------------------------|--------------------------|
| L2_cache_ECC_errors       | ECC_corrected_errors_L2_cache        |                          |
|                           | ECC_uncorrected_errors_L2_cache      |                          |
|---------------------------|--------------------------------------|--------------------------|
| memory_ECC_errors         | ECC_corrected_errors_device_mem      |                          |
|                           | ECC_uncorrected_errors_device_mem    |                          |
|---------------------------|--------------------------------------|--------------------------|
| register_file_ECC_errors  | ECC_corrected_errors_register_file   |                          |
|                           | ECC_uncorrected_errors_register_file |                          |
|---------------------------|--------------------------------------|--------------------------|
| texture_memory_ECC_errors | ECC_corrected_errors_texture_mem     |                          |
|                           | ECC_uncorrected_errors_texture_mem   |                          |
|---------------------------|--------------------------------------|--------------------------|
| PCIe_throughput           | PCIe_tx_throughput                   | NVML_PCIE_UTIL_TX_BYTES  |
|                           | PCIe_rx_throughput                   | NVML_PCIE_UTIL_RX_BYTES  |
|---------------------------|--------------------------------------|--------------------------|
| temperature               | temperature                          | NVML_TEMPERATURE_GPU     |
|---------------------------|--------------------------------------|--------------------------|
| fan_speed                 | fan_speed                            |                          |
|---------------------------|--------------------------------------|--------------------------|
| time_throttled            | time_power_capped                    | NVML_PERF_POLICY_POWER   |
|                           | time_thermal_capped                  | NVML_PERF_POLICY_THERMAL |
|---------------------------|--------------------------------------|--------------------------|