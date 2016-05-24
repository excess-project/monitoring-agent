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

/** @file mf_movidius_arduino_connector.h
 *  @brief Interface to the ARDUINO MEGA 2560 board, 
 *  which is connected to a MV0198 Daughtercard of movidius board (MV0198).
 *
 *  This interface declares a means to access measurements of the Movidius
 *  hardware component. 
 *
 *  @author Dmitry Khabi
 */

#ifndef _MOVIDIUS_ARDUINO_CONNECTOR_H
#define _MOVIDIUS_ARDUINO_CONNECTOR_H

#define MOVI_MAX_PRESET_EVENTS 16 //4 MV0198 chips, each has 4 channels
#define FORMAT_SCIENTIFIC "%2.12e"
/** @brief data structure to store MOVI monitoring data
 *
 * The data structure holds the metric names including the correspond
 * measured values. Moreover, the number of events measured is stored.
 */
typedef struct MOVI_Plugin_t MOVI_Plugin;
struct MOVI_Plugin_t
{
    char *events[MOVI_MAX_PRESET_EVENTS];
    double values[MOVI_MAX_PRESET_EVENTS];
    int num_events;
};

/** @brief Initializes the MOVI plug-in
 *
 * This function loads the underlying MOVI-C library, so that in subsequent
 * steps data can be sampled. 
 *
 * @param data structure that holds sampled MOVI events
 * @param movi_events one event for one measurement channel
 * @param num_events equals the length of MOVI_MAX_PRESET_EVENTS
 *
 * @return 1 on success; 0 otherwise.
 */

int mf_movi_init(MOVI_Plugin *data, char **movi_events, size_t num_events);
/** @brief Samples given MOVI events
 *
 * This function samples given MOVI events .
 *  After sampling, the values are stored in the data structure passed
 * to this function. Thus,
 * each call to #mf_movi_sample returns new counter values.
 *
 * @param data structure that holds sampled MOVI events
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_movi_sample(MOVI_Plugin *data);

/** @brief Initializes movidius arduino units and send to mf_server
 *
 * @it is called by mf_movi_init
 *
 * @return 1 on success; 0 otherwise.
 */
int mf_movi_unit_init(void);

/** @brief String representation of sampled MOVI events
 *
 * This functions returns a JSON representation of the MOVI events sampled. The
 * output is compatible with the EXCESS monitoring server, and thus includes
 * additional information such as timestamp of measurement. Please note that
 * a single timestamp for all events sampled by calling #mf_movi_sample.
 *
 * @param data structure that holds sample MOVI events including their values
 *
 * @return JSON document representing the given MOVI data structure
 */
char* mf_movi_to_json(MOVI_Plugin *data);

/** @brief Stops sampling MOVI events
 *
 * This methods shuts down gracefully MOVI sampling.
 */
void mf_movi_shutdown();

/** @brief Get system time **/
double timer_get_time(void);
/** @brief open arduino serial interface (over USB) **/
int open_interfase(int* fd, const char* port_name);
/** @brief set attributes for arduino serial interface (over USB) **/
int set_interface_attribs (int fd, int speed, int parity);
/** @brief set non-blocking transfer mode (over USB) **/
void set_blocking (int fd, int should_block);
void convert(MOVI_Plugin *data, char* arduino_output, size_t length,  double* ch_coef1 );
void read_arduino(int fd, char* arduino_output, size_t* length,  size_t max_length);
#endif /* _MOVI_CONNECTOR_H */
