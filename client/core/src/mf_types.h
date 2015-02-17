#ifndef MF_TYPES_H_
#define MF_TYPES_H_

/**
 * @brief pointer to the metric gathered by the plugins
 */
typedef struct metric_t* metric;

/**
 * @brief structure which represents a value of a metric
 */
struct metric_t {
    struct timespec timestamp;
    char *msg;
} metric_t;

#endif