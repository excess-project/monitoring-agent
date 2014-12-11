/**
 * @brief Defines the external API to the monitoring framework.
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */
#ifndef MF_API_H_
#define MF_API_H_


void mf_api_initialize(const char* URL, char* db_key);

/**
 * @brief Triggers the monitoring of given external function.
 */
long double mf_api_start_profiling(const char *function_name);

/**
 * @brief Stops the monitoring of the given external function.
 */
long double mf_api_stop_profiling(const char *function_name);

/**
 * @brief Query the database running on host by id in order to retrieve all
 *        metrics collected within the given range: the interval is defined by
 *        the two timestamps t0 and t1.
 */
char* get_data_by_interval(long double start_time, long double stop_time);

/**
 * @brief Query the database running on host by id in order to retrieve a
 *        specific value for the given metric.
 */
char* get_data_by_metric_by_interval(
    const char* metric_name,
    long double start_time,
    long double end_time
);

void mf_api_send(const char* json);

#endif