#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include "config.h"
#include "rgb_functions.h"

#ifndef F_CPU
#define F_CPU 16000000
#endif

#define DEBOUNCE_DELTA ((DEBOUNCE_PERIOD_MS * 1000UL) / TIMER_PERIOD)
#define LONG_PRESS_DELTA ((LONG_PRESS_PERIOD_MS * 1000UL) / TIMER_PERIOD)

#define REG_GREEN OCR0A
#define REG_RED OCR0B
#define REG_BLUE OCR1B
#define SWITCH_STATE (!(PINB & (1 << PINB2)))

#define EEPROM_ADDR_MODE (uint8_t*)0
#define EEPROM_ADDR_COLOR (uint8_t*)1

#define NUM_MODES 4

enum modes_t { OFF, RAINBOW, SLOW, FIXED };

enum modes_t mode_index = RAINBOW;
uint8_t color_index = 0;

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
  if (mode_index == FIXED) {
    color_index = (color_index + 1) % COLORS_LEN;
    eeprom_write_byte(EEPROM_ADDR_COLOR, color_index);
  }
}

void on_long_press() {
  mode_index = (mode_index + 1) % NUM_MODES;
  eeprom_write_byte(EEPROM_ADDR_MODE, mode_index);
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
  uint32_t rgb = 0;

  switch (mode_index) {
    case OFF:
      rgb = BLACK;
      break;
    case RAINBOW:
      rgb = rainbow(counter_value);
      break;
    case SLOW:
      rgb = slow_changing(counter_value);
      break;
    case FIXED:
      rgb = fixed_color(COLORS[color_index], counter_value);
      break;
  }

  REG_RED = GET_RED(rgb);
  REG_GREEN = GET_GREEN(rgb);
  REG_BLUE = GET_BLUE(rgb);
}

int main() {
  init_timers();
  init_switch();

  uint32_t local_counter;
  mode_index = eeprom_read_byte(EEPROM_ADDR_MODE);
  color_index = eeprom_read_byte(EEPROM_ADDR_COLOR);

  while (1) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { local_counter = counter; }
    handle_switch(local_counter);
    handle_leds(local_counter);
  }
}