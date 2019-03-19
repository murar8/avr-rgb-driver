#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define REG_GREEN OCR0A
#define REG_RED OCR0B
#define REG_BLUE OCR1B
#define SWITCH_STATE !(PINB & (1 << PINB2))

#define LOOP_PERIOD_MS 4
#define DEBOUNCE_TIME_MS 20
#define LONG_PRESS_TIME_MS 500
#define ANIM_PERIOD_MS 5000
#define ADVANCE_PER_LOOP UINT16_MAX / (ANIM_PERIOD_MS / LOOP_PERIOD_MS)

typedef (*on_click_function)();

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

void handle_switch() {
  static bool switch_pressed = false, switch_long_pressed = false,
              switch_state_last = false;
  static uint16_t switch_cnt = 0;

  if (switch_state_last != SWITCH_STATE) {
    switch_state_last = SWITCH_STATE;
    switch_cnt = 0;
    return;
  }

  if (switch_cnt < LONG_PRESS_TIME_MS / LOOP_PERIOD_MS) {
    ++switch_cnt;
  } else if (!switch_long_pressed && switch_state_last) {
    REG_RED ^= 0xff;  // code
    switch_long_pressed = true;
  }

  if (switch_cnt < DEBOUNCE_TIME_MS / LOOP_PERIOD_MS) {
    return;
  }

  if (switch_state_last) {
    if (!switch_pressed) {
      REG_BLUE ^= 0xff;  // code
      switch_pressed = true;
    }
  } else {
    switch_pressed = false;
    switch_long_pressed = false;
  }
}

int main() {
  init_timers();
  init_switch();
  while (1) {
    handle_switch();
    _delay_ms(LOOP_PERIOD_MS);
  }
}