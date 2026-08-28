#include "scheduler.h"
#include "timer.h"
#include <avr/io.h>
void red_led(void *arg);
void white_led(void *arg);

int main(void) {
  struct task A;
  struct task B;
  // Task Creation
  task_create(&A, &red_led, 1, &B);
  task_create(&B, &white_led, 0, &A);
  timer_init(1);
  scheduler_start();
}

void red_led(void *arg) {
  while (1) {
    DDRD |= (1 << 7);
    PORTD |= (1 << 7);
    DDRD &= ~(1 << 6);
    PORTD &= ~(1 << 6);
  }
}

void white_led(void *arg) {
  while (1) {
    DDRD &= ~(1 << 7);
    PORTD &= ~(1 << 7);
    DDRD |= (1 << 6);
    PORTD |= (1 << 6);
  }
}
