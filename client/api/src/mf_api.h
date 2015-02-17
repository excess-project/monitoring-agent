/**
 * @brief Defines the external API to the monitoring framework.
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */
#ifndef MF_API_H_
#define MF_API_H_


/**
 * @brief Initializes the monitoring; has to be called in advance. On success,
 *        the function call returns the current execution id.
 */
char* mf_api_initialize(const char* URL);

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

/**
 * @brief Returns the execution id of the given application. It should be noted
 *        that mf_api_initialize(URL) has to be called first.
 */
char* mf_api_get_execution_id();

/**
 * @brief Returns data stored in the database related to the given execution id.
 *        The format the data is returned is JSON formatted.
 */
char* mf_api_get_data_by_id(char* execution_id);

#endif