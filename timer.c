#include "timer.h"
#include "scheduler.h"
#include <avr/common.h>
#include <avr/io.h>
void timer_init(void) {

  // Set the timer to normal
  TCCR0A |= (1 << 1);
  TCCR0A &= ~(1 << 0);
  TCCR0B &= ~((1 << 3));

  // Set the context switch to occur in roughly ~10ms
  TCCR0B |= (1 << 2);
  TCCR0B &= ~(1 << 1);
  TCCR0B |= (1 << 0);
  OCR0A = 158;
  // Set the interrupts on OCR0A match
  TIMSK0 |= (1 << 1);
  TIFR0 |= (1 << 1);
  context_fill();
  // Set global interrupts
  SREG |= (1 << 7);
  scheduler_start();
  while (1) {
  }
}
