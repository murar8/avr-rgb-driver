#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "rgb_functions.h"

#define REG_GREEN OCR0A
#define REG_RED OCR0B
#define REG_BLUE OCR1B
#define SWITCH_STATE !(PINB & (1 << PINB2))

#define LOOP_PERIOD_MS 4
#define DEBOUNCE_TIME_MS 20
#define LONG_PRESS_TIME_MS 500
#define ANIM_PERIOD_MS 5000
#define ADVANCE_PER_LOOP UINT16_MAX / (ANIM_PERIOD_MS / LOOP_PERIOD_MS)


uint8_t current_function, current_mode = 0;

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

void on_press() {
  switch (current_mode) {
    case 0:
      if (current_function >= ARRAY_LEN(RGB_FUNCTIONS_MODE0) - 1) {
        current_function = 0;
        return;
      }
    case 1:
      if (current_function >= ARRAY_LEN(RGB_FUNCTIONS_MODE1) - 1) {
        current_function = 0;
        return;
      }
  }
  ++current_function;
}

void on_long_press() {
  current_mode = !current_mode;
  current_function = 0;
}

void handle_switch() {
  static uint16_t switch_cnt = 0;

  if (switch_cnt < UINT16_MAX) {
    ++switch_cnt;
  }

  if (SWITCH_STATE) {
    if (switch_cnt == LONG_PRESS_TIME_MS / LOOP_PERIOD_MS) {
      on_long_press();
    }
  } else {
    if (switch_cnt > DEBOUNCE_TIME_MS / LOOP_PERIOD_MS &&
        switch_cnt < LONG_PRESS_TIME_MS / LOOP_PERIOD_MS) {
      on_press();
    }
    switch_cnt = 0;
  }
}

void animate_leds() {
  static uint8_t step = 0;

  RGB_state state;
  switch (current_mode) {
    case 0:
      state = RGB_FUNCTIONS_MODE0[current_function](step);
    case 1:
      state = RGB_FUNCTIONS_MODE1[current_function](step);
  }
  REG_RED = state.red;
  REG_GREEN = state.green;
  REG_BLUE = state.blue;

  ++step;
}

int main() {
  init_timers();
  init_switch();
  while (1) {
    handle_switch();
    animate_leds();
    _delay_ms(LOOP_PERIOD_MS);
  }
}