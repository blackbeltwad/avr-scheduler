#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_SIZE 256
#define MAX_STACKS 5
#define SPH (*(volatile unsigned char *)0x5E)
#define POP_SIZE 34
#include <stdint.h>

// Stack Grows Down , POP is increment, PUSH is decrement
enum task_state { TASK_READY, TASK_RUNNING, TASK_BLOCKED, TASK_SLEEPING };

struct task {
  volatile uint8_t stack[MAX_SIZE];
  volatile uint8_t *stack_pointer;
  uint8_t priority;
  enum task_state state;
};

struct scheduler {
  struct task *tasks[MAX_STACKS];
  struct task *current_task;
  int8_t task_index;
  int8_t total_tasks;
};

void task_create(struct task *task, void (*entry_point)(void));
uint16_t store_and_pop_stack_pointer(uint16_t stack_address);
void scheduler_start(void);
#endif
