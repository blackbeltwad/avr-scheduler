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
    this_task->owns_mutex = 1;
    sei();
  } else {
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
    mutex->owner = NULL;
    this_task->owns_mutex = 0;
  }
  sei();
}
