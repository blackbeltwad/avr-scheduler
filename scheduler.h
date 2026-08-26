#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_STACKS 5
#define MAX_SIZE 256
#include <stdint.h>

enum task_state { TASK_READY, TASK_RUNNING, TASK_BLOCKED, TASK_SLEEPING };

struct task {
  volatile uint8_t stack[MAX_SIZE];
  volatile uint8_t *stack_pointer;
  uint8_t priority;
  enum task_state state;
  double sleep_remaining;
};

typedef struct task task;
void scheduler_start(void);
void task_create(struct task *task, void (*entry_point)(void),
                 uint8_t priority_value);
#endif
