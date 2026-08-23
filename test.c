#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

void turn_on(void);
void turn_off(void);

int main() {
  DDRD |= (1 << 7);
  PORTD |= (1 << 7);

  while (1) {
    turn_on();
    turn_off();
  }
}

void turn_on(void) {
  PORTD |= (1 << 7);
  _delay_ms(2000);
}

void turn_off(void) {
  PORTD &= ~(1 << 7);
  _delay_ms(2000);
}
