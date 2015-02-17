/**
 * @brief Defines the external API to the monitoring framework.
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */
#ifndef MF_API_H_
#define MF_API_H_


/**
 * @brief Initializes the monitoring; has to be called in advance.
 */
void mf_api_initialize(const char* URL);

/**
 * @brief Starts the monitoring of given external function.
 */
long double mf_api_start_profiling(const char *function_name);

/**
 * @brief Sends data formatted in a JSON-like format using key-value pairs.
 */
void mf_api_send(const char* json);

/**
 * @brief Stops the monitoring of the given external function.
 */
long double mf_api_stop_profiling(const char *function_name);

/**
 * @brief Query the database in order to retrieve all metrics collected
 *        within the given range: the interval is defined by the two
 *        timestamps start_time and stop_time.
 */
char* get_data_by_interval(long double start_time, long double stop_time);

#endif