#include "scheduler.h"
#include "timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#define MAX_SIZE 128

// Define Globals

struct Current_Task ptrs = {.stacks = {0}, .cur = -1, .max = -1};

// State machine for what functions runs next
void Task_Create(struct Task *task, void (*Func)(void)) {

  ptrs.cur++;
  ptrs.max++;
  task->sp = &task->stack[127];

  // Point stack at top and put return address there for PC
  task->stack[0] = 0xFF; // Set a canary
  *(void (**)(void))(task->sp) = Func;
  ptrs.stacks[ptrs.cur] = task->sp;
}

ISR(TIMER1_COMPA_vect) {
  context_swtich_push();
  ptrs.stacks[ptrs.cur] = (volatile uint16_t *)((SPH << 8) | (SPL));
  if (ptrs.cur == ptrs.max) {
    ptrs.cur = 0;
  }
  ptrs.cur++;
  // Change stacks
  SPH = (volatile uint16_t)(ptrs.stacks[ptrs.cur]) >> 8;
  SPL = ((volatile uint16_t)(ptrs.stacks[ptrs.cur])) & 0x0F;
  // Pop it off
  context_switch_pop();
}
