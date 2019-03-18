/**
 * main.c
 *
 * Control the RGB LED strip for the wraith spire CPU cooler.
 *
 * AUTHOR: Lorenzo Murarotto
 **/

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define REG_GREEN OCR0A
#define REG_RED OCR0B
#define REG_BLUE OCR1B
#define SWITCH_PRESSED !(PINB & (1 << PINB2))

#define ANIM_PERIOD_MS 5000  // >= DEBOUNCE_REG_RED = 0; * 255
#define DEBOUNCE_TIME_MS 16
#define DEBOUNCE_STEPS 4

const uint8_t sine_rgb[] = {
    128, 134, 140, 146, 152, 158, 165, 170, 176, 182, 188, 193, 198, 203, 208,
    213, 218, 222, 226, 230, 234, 237, 240, 243, 245, 248, 250, 251, 253, 254,
    254, 255, 255, 255, 254, 254, 253, 251, 250, 248, 245, 243, 240, 237, 234,
    230, 226, 222, 218, 213, 208, 203, 198, 193, 188, 182, 176, 170, 165, 158,
    152, 146, 140, 134, 128, 121, 115, 109, 103, 97,  90,  85,  79,  73,  67,
    62,  57,  52,  47,  42,  37,  33,  29,  25,  21,  18,  15,  12,  10,  7,
    5,   4,   2,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   1,   1,   2,   4,   5,   7,   10,  12,  15,  18,  21,  25,  29,  33,
    37,  42,  47,  52,  57,  62,  67,  73,  79,  85,  90,  97,  103, 109, 115,
    121};

bool pressed_already, button_pressed = false;

void init_timers() {
  DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB4);

  TCCR0A |= (1 << COM0A1) |
            (1 << COM0B1) |  // Clear OCOA and OC0B on compare match
            (1 << WGM01) | (1 << WGM00);  // Fast PWM, TOP=0xFF
  TCCR0B |= (1 << CS00);                  // CLKio/1, F=62500Hz

  TCCR1 |= (1 << CS10);    // CLKio/1, F=62500Hz
  GTCCR |= (1 << PWM1B) |  // Enable pulse width modulator with TOP = OCR1C
           (1 << COM1B1);  // clear OC1B on compare match
  OCR1C |= 0xFF;
}

void init_switch() {
  PORTB |= (1 << PB2);  // Pull up PB2
  DDRB |= (1 << PB3);   // Use PB3 as GND
}

void rainbow(uint8_t step) {
  REG_RED = sine_rgb[step];
  REG_GREEN = sine_rgb[(step + 85) % 255];
  REG_BLUE = sine_rgb[(step + 170) % 255];
}

void white() { REG_RED = REG_BLUE = REG_GREEN = 0xFF; }

/**
 * Sets the button_pressed flag to true if PB2 was driven low for
 *DEBOUNCE_TIME_MS.
 **/
void read_switch() {
  bool changed = false;
  uint8_t value = SWITCH_PRESSED;

  for (uint8_t i = 0; i < DEBOUNCE_STEPS; ++i) {
    _delay_ms(DEBOUNCE_TIME_MS / DEBOUNCE_STEPS);

    if (SWITCH_PRESSED != value) {
      changed = true;
    }
  }
  if (changed) {
    return;
  }
  if (value && !pressed_already) {
    button_pressed = true;
    pressed_already = true;
  } else {
    pressed_already = false;
  }
}

int main() {
  init_timers();
  init_switch();

  uint8_t step = 0;

  while (1) {
    read_switch();
    if (button_pressed) {
      REG_RED ^= 0xFF;
      button_pressed = false;
    }

    _delay_ms((ANIM_PERIOD_MS / 255) - DEBOUNCE_TIME_MS);
  }
}
