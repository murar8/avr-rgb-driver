#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include "config.h"
#include "rgb_functions.h"

#define REG_GREEN OCR0A
#define REG_RED OCR0B
#define REG_BLUE OCR1B

#define SWITCH_STATE (!(PINB & (1 << PINB2)))
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

uint8_t color_index;

volatile uint32_t counter = 0;

ISR(TIM1_OVF_vect) { ++counter; }

void init_timers() {
  DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB4);

  TCCR0A |= (1 << COM0A1) |
            (1 << COM0B1) |  // Clear OCOA and OC0B on compare match
            (1 << WGM00);    //  Phase Correct PWM, TOP=0xFF
  TCCR0B |= (1 << CS00);     // CLKio/1, F=~31372Hz

  TCCR1 |= (1 << CS11);    // CLKio/2, F=31250Hz
  GTCCR |= (1 << PWM1B) |  // Enable pulse width modulator with TOP = OCR1C
           (1 << COM1B1);  // clear OC1B on compare match
  OCR1C |= 0xFF;

  TIMSK |= (1 << TOIE1);  // Enable overflow interrupt
  sei();
}

void init_switch() {
  PORTB |= (1 << PB2);  // Pull up PB2
  DDRB |= (1 << PB3);   // Use PB3 as GND
}

void on_press() {
  if (color_index < ARRAY_LEN(COLORS) - 1) {
    ++color_index;
  } else {
    color_index = 0;
  }
}

void on_long_press() {
  /*if (mode >= FIXED) {
    mode = OFF;
  } else {
    ++mode;
  }*/
}

void handle_switch(uint32_t counter_value) {
  static uint32_t counter_start = 0;
  static uint8_t long_pressed = 0;

  uint32_t delta = counter_value - counter_start;

  if (!SWITCH_STATE) {
    if (delta >= DEBOUNCE_DELTA && !long_pressed) {
      on_press();
    }
    counter_start = counter_value;
    long_pressed = 0;
  } else {
    if (delta >= LONG_PRESS_DELTA && !long_pressed) {
      on_long_press();
      long_pressed = 1;
    }
  }
}

void handle_leds(uint32_t counter_value) {
  // uint8_t step = ((counter_value % ANIMATION_DELTA) * UINT8_MAX) /
  // ANIMATION_DELTA;
  //uint32_t rgb = fixed_color(COLORS[color_index], counter_value);

  //REG_RED = GET_RED(rgb);
  //REG_GREEN = GET_GREEN(rgb);
  //REG_BLUE = GET_BLUE(rgb);
}

int main() {
  init_timers();
  init_switch();

  uint32_t local_counter;

  while (1) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { local_counter = counter; }
    handle_switch(local_counter);
    handle_leds(local_counter);
  }
}