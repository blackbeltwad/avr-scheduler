#include "scheduler.h"
#include "timer.h"
#include <avr/io.h>
void red_led(void);
void white_led(void);

int main(void) {
  struct task A;
  struct task B;
  // Task Creation
  task_create(&A, &red_led);
  task_create(&B, &white_led);
  timer_init();
}

void red_led(void) {
  while (1) {
    DDRD |= (1 << 7);
    PORTD |= (1 << 7);
    DDRD &= ~(1 << 6);
    PORTD &= ~(1 << 6);
  }
}

void white_led(void) {
  while (1) {
    DDRD &= ~(1 << 7);
    PORTD &= ~(1 << 7);
    DDRD |= (1 << 6);
    PORTD |= (1 << 6);
  }
}
