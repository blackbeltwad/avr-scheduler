#include "state.h"
#include "scheduler.h"
#include <avr/io.h>
void task_block(struct task *task) { task->state = TASK_BLOCKED; }

void task_wake(struct task *task) { task->state = TASK_READY; }

void task_sleep(struct task *task, double time_ms) {
  task->state = TASK_SLEEPING;
  task->sleep_remaining = time_ms;
  // Force a context switch with TIFR1 and reset compA to give more time before
  // next interrupt cycle
  OCR1AH = 0;
  OCR1AL = 0;
  TIFR1 |= (1 << 1);
}

void task_yield(void) {
  // Force context switch
  OCR1AL = 0;
  OCR1AL = 0;
  TIFR1 |= (1 << 1);
}
