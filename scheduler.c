#include "scheduler.h"
#include "timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#define MAX_SIZE 128

// State machine for what functions runs next
void Task_Create(struct Task *task, struct Task *taskB, void (*set)(void)) {

  // Point stack at top and put return address there for PC
  task->stack[0] = 0xFF; // Set a canary

  (task->sp) = &(task->stack[127]);
  *(void (**)(void))(task->sp) = set;

  // Move it down again store the next
  task->sp--;
  task->sp = (volatile uint16_t *)&taskB->sp;
}

ISR(TIMER1_COMPA_vect) {}
