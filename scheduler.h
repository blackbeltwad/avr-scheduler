#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_SIZE 128
#include <stdint.h>
// Stack Grows Down , POP is increment, PUSH is decrement
struct Task {
  volatile uint16_t stack[MAX_SIZE];
  volatile uint16_t *sp;
};
void Task_Create(struct Task *task, struct Task *taskB, void (*set)(void));
#endif
