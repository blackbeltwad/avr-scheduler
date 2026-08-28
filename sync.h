#ifndef SYNC_H
#define SYNC_H
#include "scheduler.h"
#include <stddef.h>
typedef struct mutex_t {
  struct task *owner;
} mutex_t;

mutex_t uart_mutex = {.owner = NULL};
void lock_mutex(mutex_t *mutex);
void unlock_mutex(mutex_t *mutex);
#endif
