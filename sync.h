#ifndef SYNC_H
#define SYNC_H
#include "scheduler.h"
#include <stddef.h>
#include <stdint.h>
typedef struct mutex_t {
  struct task *owner;
  struct task *block_list[MAX_STACKS];
  uint8_t block_index;
} mutex_t;

mutex_t uart_mutex = {.owner = NULL, .block_list = NULL, .block_index = 0};
void lock_mutex(mutex_t *mutex);
void unlock_mutex(mutex_t *mutex);
#endif
