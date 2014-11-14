#ifndef PUBLISHER_H_
#define PUBLISHER_H_

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

int publish(const char *url, Message *messages);

#endif
