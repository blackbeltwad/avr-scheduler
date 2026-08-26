#ifndef STATE_H
#define STATE_H
#include "scheduler.h"
#include <stdint.h>
void task_sleep(struct task *task, double time_ms);
void task_wake(struct task *task);
void task_block(struct task *task);
void task_yield(void);
void set_priority(struct task *task, uint8_t priority_value);

#endif
