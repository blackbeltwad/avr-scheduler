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

  task->stack[0] = 0xFF; // Set a canary

  task->sp = task->stack; // Set the stack pointer
  task->sp += 254;

  // PC needs an adress to give cpu
  *(int (**)(void))(task->sp) = func;

  // Might add garbage values depending on context switch configuration
  return 0;
}
