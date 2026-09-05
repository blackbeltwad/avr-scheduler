#include "scheduler.h"
#include "timer.h"
#include <avr/io.h>
#include <stdint.h>
void red_led(void *arg);
void white_led(void *arg);
void blue_led(void *arg);
void set_only_pin(uint8_t pin);
int main(void) {
  struct task A;
  struct task B;
  struct task C;
  // Task Creation
  task_create(&C, &blue_led, 2, 0);
  task_create(&A, &red_led, 1, &B);
  task_create(&B, &white_led, 1, &A);
  timer_init(1000);
  scheduler_start();
}
void red_led(void *arg) {
  while (1) {
    set_only_pin(7);
  }
}

void white_led(void *arg) {
  while (1) {
    set_only_pin(6);
  }
}

void blue_led(void *arg) {
  while (1) {
    task_yield();
    set_only_pin(5);
  }
}

void set_only_pin(uint8_t pin) {
  DDRD = (1 << pin);
  PORTD = (1 << pin);
}
