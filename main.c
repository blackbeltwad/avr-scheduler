#include "scheduler.h"
#include "timer.h"

void FuncA(void);
void FuncB(void);
void FuncC(void);

int main(void) {
  struct Task A;
  struct Task B;
  struct Task C;
  // Task Creation
  Task_Create(&A, &FuncA);
  Task_Create(&B, &FuncB);
  Task_Create(&C, &FuncC);
  timer_init();
}

void FuncA(void) {
  while (1) {
  }
}

void FuncB(void) {
  while (1) {
  }
}

void FuncC(void) {
  while (1) {
  }
}
