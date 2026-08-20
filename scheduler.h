#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_SIZE 256
#define MAX_STACKS 5
#include <stdint.h>

// Stack Grows Down , POP is increment, PUSH is decrement
struct Task {
  volatile uint8_t stack[MAX_SIZE];
  volatile uint16_t *sp;
};

struct Current_Task {
  volatile uint16_t *stacks[MAX_STACKS];
  volatile int8_t cur;
  volatile int8_t max;
};

void Task_Create(struct Task *task, void (*Func)(void));
extern void context_switch_push(void);
extern void context_switch_pop(void);
#endif
