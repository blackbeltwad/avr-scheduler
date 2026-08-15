#include <avr/interrupt.h>
#include <stdint.h>
#define MAX_SIZE 128;

// Stack Grows Down , POP is increment, PUSH is decrement
struct Task {
  volatile uint8_t stack[255];
  volatile uint8_t *sp;
};

// State machine for what functions runs next
int Task_Create(struct Task *task, int (*func)(void)) {
  // Set a canary
  task->stack[0] = 0xFF;

  // Set the stack pointer
  task->sp = task->stack;
  task->sp += 254;

  // Need to give fake info on the first context switch
  *(int (**)(void))(task->sp) = func;

  // Might add garbage values depending on context switch configuration
  return 0;
}
