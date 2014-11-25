#ifndef PUBLISHER_H_
#define PUBLISHER_H_

#define SEND_SUCCESS 1
#define SEND_FAILED  0

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

char* get_execution_id(const char *URL, char *message);

int publish(const char *URL, Message *messages);

int publish_json(const char *URL, char *message);

void shutdown_curl(); // FIXME: CURL should be hidden



#endif
