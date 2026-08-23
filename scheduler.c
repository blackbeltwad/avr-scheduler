#include "scheduler.h"
#include "timer.h"
#include <avr/common.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#define MAX_SIZE 256
#define SPH (*(volatile unsigned char *)0x5E)
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

void context_fill(void) {
  // Move SP of tasks to correct spot; that is not being PUSHED FIRST
  for (int8_t i = 0; i < ptrs.max; i++) {
    if (i != ptrs.cur) {
      ptrs.stacks[i] = ptrs.stacks[i] - 33;
      ptrs.stacks[i] = 0x0000; // Make sure SREG has all flags OFF
    }
  }
}
void scheduler_start(void) {
  // Get the high and low bytes of the function pointer that is stored on the
  // STACK
  uint16_t h = *(uint16_t *)ptrs.stacks[ptrs.cur];
  uint16_t l = *(uint16_t *)(ptrs.stacks[ptrs.cur] + 1);
  void (*start)(void) = (void(*))((h << 8) | l);

  // Set stack pointer to end of the TASK[255]
  SPH = (ptrs.stacks[ptrs.cur] - 1) >> 8;
  SPL = ptrs.stacks[ptrs.cur] - 1;
  start();
}

void context_switch(uint16_t address) {
  // Use built in SP to increase or decrease pointer
  ptrs.stacks[ptrs.cur] = address;
  if (ptrs.cur == ptrs.max) {
    ptrs.cur = 0;
  } else {
    ptrs.cur++;
  }
  // Change stack pointers
  SPH = (uint8_t)(ptrs.stacks[ptrs.cur] >> 8);
  SPL = (uint8_t)(ptrs.stacks[ptrs.cur] & 0x00FF);
}
