#include "timer.h"
#include "scheduler.h"
#include <avr/common.h>
#include <avr/io.h>
void timer_init(void) {

  // Set the timer to normal
  TCCR1A &= ~(1 << 1);
  TCCR1A &= ~(1 << 0);
  TCCR1B |= ((1 << 3));
  TCCR1B &= ~(1 << 4);
  // Set the context switch to occur in roughly ~10ms

  TCCR1B |= (1 << 2);
  TCCR1B &= ~(1 << 1);
  TCCR1B |= (1 << 0);
  OCR1AH = 0x0F;
  OCR1AL = 0x42;

  // Set the interrupts on OCR0A match
  TIMSK1 |= (1 << 1);
  TIFR1 |= (1 << 1);
  // Set global interrupts
  SREG |= (1 << 7);
  scheduler_start();
  while (1) {
  }
}
