#include "sync.h"
#include "scheduler.h"
#include <avr/interrupt.h>
#include <stddef.h>
#include <stdint.h>

void lock_mutex(mutex_t *mutex) {
  cli();
  struct task *this_task = get_current_task();

  if (mutex->owner == NULL) {
    mutex->owner = this_task;
    sei();
  } else {
    if (this_task->priority > mutex->owner->priority) {
      mutex->owner->priority = this_task->priority; // boost
    }
    this_task->state = TASK_BLOCKED;
    mutex->block_list[mutex->block_index] = this_task;
    mutex->block_index = (mutex->block_index + 1) % MAX_STACKS;
    task_yield();
    sei();
  }
}

void unlock_mutex(mutex_t *mutex) {
  cli();
  struct task *this_task = get_current_task();
  if (this_task == mutex->owner) {
    uint8_t highest_priority = 0;
    struct task *highest_priority_task = NULL;
    int winner_index = -1;

    for (int i = 0; i < MAX_STACKS; i++) {
      if (mutex->block_list[i] != NULL &&
          mutex->block_list[i]->priority >= highest_priority) {
        highest_priority = mutex->block_list[i]->priority;
        highest_priority_task = mutex->block_list[i];
        winner_index = i;
      }
    }

    mutex->owner = highest_priority_task;
    mutex->owner->priority = mutex->owner->base_priority;

    if (highest_priority_task != NULL) {
      mutex->block_list[winner_index] = NULL; // remove them
      task_unblock(highest_priority_task);    // actually wake them
    }
  }
  sei();
}
