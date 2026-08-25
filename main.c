#include "scheduler.h"
#include "timer.h"
#include <avr/io.h>
void FuncA(void);
void FuncB(void);

int main(void) {
  struct Task A;
  struct Task B;
  // Task Creation
  Task_Create(&A, &FuncA);
  Task_Create(&B, &FuncB);
  timer_init();
}

void FuncA(void) {
  while (1) {
    DDRD |= (1 << 7);
    PORTD |= (1 << 7);
  }
}

void FuncB(void) {
  while (1) {
    DDRD &= ~(1 << 7);
    PORTD &= ~(1 << 7);
  }
}
