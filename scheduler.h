#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_SIZE 256
#define MAX_STACKS 5
#define SPH (*(volatile unsigned char *)0x5E)
#include <stdint.h>

// Stack Grows Down , POP is increment, PUSH is decrement
struct Task {
  volatile uint8_t stack[MAX_SIZE];
  volatile uint16_t *sp;
};

struct Current_Task {
  volatile uint16_t stacks[MAX_STACKS];
  volatile int8_t cur;
  volatile int8_t max;
};

void Task_Create(struct Task *task, void (*Func)(void));
void context_switch(uint16_t address);
#endif
