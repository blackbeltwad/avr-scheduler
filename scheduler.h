#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_STACKS 5
#define MAX_SIZE 256
#include <stdint.h>

enum task_state { TASK_READY, TASK_RUNNING, TASK_BLOCKED, TASK_SLEEPING };

struct task {
  volatile uint8_t stack[MAX_SIZE];
  volatile uint8_t *stack_pointer;
  uint8_t priority; // Priority stat that gets bumped
  volatile void *task_arg;
  enum task_state state;
  double sleep_remaining;
  uint8_t base_priority; // Regular priority
};

void scheduler_start(void);
void task_create(struct task *task, void (*entry_point)(void *),
                 uint8_t priority_value, void *entry_argument);
void task_sleep(double time_ms);
void task_wake();
void task_block(void *arg);
void task_yield(void);
void set_priority(uint8_t priority_value);
void task_unblock(void *arg);
struct task *get_current_task(void);
#endif
