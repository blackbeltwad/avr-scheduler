#include <stdint.h>

#define ADMUX (*(volatile unsigned char *)0x7C)
#define ADCSRA (*(volatile unsigned char *)0x7A)
#define ADCH (*(volatile unsigned char *)0x79)
#define ADCL (*(volatile unsigned char *)0x78)
// UART CONFIGURATION -------------
#define UCSR0A (*(volatile unsigned char *)0xC0)
#define UCSR0B (*(volatile unsigned char *)0xC1)
#define UCSR0C (*(volatile unsigned char *)0xC2)
#define UBRR0L (*(volatile unsigned char *)0xC4)
#define UBRR0H (*(volatile unsigned char *)0xC5)
#define UDR0 (*(volatile unsigned char *)0xC6)
// --------------------------------------
// PWM CONFIG
#define TCCR0A (*(volatile unsigned char *)0x44)
#define TCCR0B (*(volatile unsigned char *)0x45)
#define OCR0A (*(volatile unsigned char *)0x47)
#define OCR0B (*(volatile unsigned char *)0x48)
// --------------------------------------
#define DDRD (*(volatile unsigned char *)0x2A)
#define CLEAR(reg, bit) ((reg) &= ~(1 << (bit)))
#define SET(reg, bit) ((reg) |= (1 << (bit)))
#define PORTD (*(volatile unsigned char *)0x2B)

// Prototype Functions
void pwm_init();
void adc_init();
void uart_init();
void int_to_char(uint8_t *val, char arr[]);

int main(void) {
  pwm_init();
  adc_init();
  uart_init();

  //  ASCII Array
  char arr[4] = {0};
  // Need to account for adc noise
  uint8_t saved = 0;

  // Wait for conversion to finish
  while (1) {
    SET(ADCSRA, 4); // clear ADIF
    SET(ADCSRA, 6); // start conversion
    while (!(ADCSRA & (1 << 4))) {
    }
    // Read 10-bit ADC result
    uint16_t value = ADCL | ((uint16_t)ADCH << 8);

    // Convert ADC result to pwm
    // Drop the last 2 bits saves cpu cycles.
    uint8_t pwm = value >> 2;

    OCR0A = pwm;
    uint8_t diff;
    if (pwm > saved) {
      diff = pwm - saved;
    } else {
      diff = saved - pwm;
    }

    // Account for ADC Noise with diff
    if (diff > 1) {

      saved = pwm;
      int_to_char(&pwm, arr);
      // Wait until UART data register is empty
      for (int i = 0; i < 4; i++) {

        while (!(UCSR0A & (1 << 5))) {
        }
        // Send PWM byte
        UDR0 = arr[i];
      }
    }
  }
}

void int_to_char(uint8_t *val, char arr[]) {
  arr[0] = (*val / 100) + '0';
  arr[1] = (*val / 10 % 10) + '0';
  arr[2] = (*val % 10) + '0';
  arr[3] = '\n';
}

void pwm_init() {
  TCCR0A = (1 << 7) | (1 << 1) | (1 << 0);
  TCCR0B = (1 << 1) | (1 << 0);
  SET(DDRD, 6);
}

void adc_init() {
  // ADC0
  CLEAR(ADMUX, 3);
  CLEAR(ADMUX, 2);
  CLEAR(ADMUX, 1);
  CLEAR(ADMUX, 0);

  // Right adjust
  CLEAR(ADMUX, 5);

  // AVCC reference
  CLEAR(ADMUX, 7);
  SET(ADMUX, 6);

  // Enable ADC
  SET(ADCSRA, 7);

  // ADC clock = CPU / 128
  SET(ADCSRA, 2);
  SET(ADCSRA, 1);
  SET(ADCSRA, 0);
}

void uart_init() {
  // Set the enabler
  SET(UCSR0B, 3);

  // Set USART to Asynchrnous
  CLEAR(UCSR0C, 7);
  CLEAR(UCSR0C, 6);

  // Set the # of Stop bits
  CLEAR(UCSR0C, 3);

  // Set number of data bits for transmission
  CLEAR(UCSR0B, 2);
  SET(UCSR0C, 2);
  SET(UCSR0C, 1);

  // Set the baud rate to 4000
  UBRR0H = 0;
  UBRR0L = 8;
}
