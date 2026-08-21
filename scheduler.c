#include "scheduler.h"
#include "timer.h"
#include <avr/common.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#define MAX_SIZE 256

// Define Globals

struct Current_Task ptrs = {.stacks = {0}, .cur = -1, .max = -1};

// State machine for what functions runs next
void Task_Create(struct Task *task, void (*Func)(void)) {

  ptrs.cur++;
  ptrs.max++;
  task->sp = (volatile uint16_t *)&task->stack[255];

  // Point stack at top and put return address there for PC
  task->stack[0] = 0xFF; // Set a canary
  uint16_t f = (uint16_t)(Func);
  volatile uint8_t h = (volatile uint8_t)(f >> 8);
  volatile uint8_t l = (volatile uint8_t)(f);
  *(volatile uint8_t *)task->sp = l;
  task->sp = (volatile uint16_t *)((volatile uint8_t *)task->sp - 1);
  *(volatile uint8_t *)task->sp = h;
  ptrs.stacks[ptrs.cur] = (uint16_t)task->sp;
}

void context_switch(void) {
  // Use built in SP to increase or decrease pointer

  ptrs.stacks[ptrs.cur] = (uint16_t)SP;
  if (ptrs.cur == ptrs.max) {
    ptrs.cur = 0;
  } else {
    ptrs.cur++;
  }
  // Change stack pointers
  SP = (uint16_t)ptrs.stacks[ptrs.cur];
}
