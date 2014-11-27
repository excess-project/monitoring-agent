/**
 * @brief Publish a message by posting it to user-defined URL
 *
 * The @<Publisher@> is responsible for sending data to user-defined URLs. The
 * current implementation uses cURL to publish string messages. The monitoring
 * framework uses during its prototyping stage the function @<publish_json@> in
 * order to send performance and/or energy counters to the server via JSON.
 *
 * @author Dennis Hoppe <hoppe@hrls.de>
 */

#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#define SEND_SUCCESS 1
#define SEND_FAILED  0
#define ID_SIZE 64

extern char execution_id[ID_SIZE];

typedef struct Message_t Message;
typedef struct Data_t Data;

struct Data_t {
  char *key;
  char *value;
};

struct Message_t {
  char *sender;
  char *username;
  char *timestamp;
  Data *data;
};

/**
 * @brief Returns the execution id used for communication with Elasticsearch.
 *
 * If the variable #execution_id is set, then this function returns the already 
 * initialized value of #execution_id. Otherwise, this function queries 
 * the server via calling publish_json() based on the given URL. The #message
 * must have the following format in JSON:
 *
 * \code{.c}
 * {
 *     "Name": <hostname>,
 *     "Description": <description of the job>,
 *     "Start_date": <timestamp>,
 *     "Username": <username>
 * }
 * \endcode
 */
char* get_execution_id(const char *URL, char *message);

/**
 * @brief Publishes previously defined Message objects.
 *
 * It should be noted that this function is not yet implemented.
 *
 * @return 1
 */
int publish(const char *URL, Message *messages);

/**
 * @brief Sends the data defined in message to the given URL via cURL.
 *
 * @return 1 if successful; 0 otherwise
 */
int publish_json(const char *URL, char *message);

/**
 * @brief Frees the cURL headers and global variables.
 *
 * Note: This method needs to be private in order to hide cURL.
 */
void shutdown_curl();

#endif
