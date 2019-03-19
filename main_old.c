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

#define LOOP_PERIOD_MS 4
#define DEBOUNCE_TIME_MS 20
#define ANIM_PERIOD_MS 5000
#define ADVANCE_PER_LOOP UINT16_MAX / (ANIM_PERIOD_MS / LOOP_PERIOD_MS)
#define RGB_BLEND_FACTOR 200

typedef void (*rgb_function)(uint8_t);
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

const uint8_t sine_uint8[] = {
    0,   0,   0,   1,   1,   1,   2,   2,   3,   4,   5,   5,   6,   7,   9,
    10,  11,  12,  14,  15,  17,  18,  20,  21,  23,  25,  27,  29,  31,  33,
    35,  37,  40,  42,  44,  47,  49,  52,  54,  57,  59,  62,  65,  67,  70,
    73,  76,  79,  82,  85,  88,  90,  93,  97,  100, 103, 106, 109, 112, 115,
    118, 121, 124, 128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162,
    165, 167, 170, 173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203,
    206, 208, 211, 213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235,
    237, 238, 240, 241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252, 253,
    253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253,
    253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 238, 237,
    235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 215, 213, 211, 208, 206,
    203, 201, 198, 196, 193, 190, 188, 185, 182, 179, 176, 173, 170, 167, 165,
    162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118,
    115, 112, 109, 106, 103, 100, 97,  93,  90,  88,  85,  82,  79,  76,  73,
    70,  67,  65,  62,  59,  57,  54,  52,  49,  47,  44,  42,  40,  37,  35,
    33,  31,  29,  27,  25,  23,  21,  20,  18,  17,  15,  14,  12,  11,  10,
    9,   7,   6,   5,   5,   4,   3,   2,   2,   1,   1,   1,   0,   0,   0,
    0};

uint8_t sine_rgb(uint8_t step) {
  if (step > RGB_BLEND_FACTOR) return 0;
  return sine_uint8[((uint16_t)step * UINT8_MAX) / RGB_BLEND_FACTOR];
}

void rainbow(uint8_t step) {
  REG_RED = sine_rgb(step);
  REG_GREEN = sine_rgb((step + 85) % 255);
  REG_BLUE = sine_rgb((step + 170) % 255);
}

void white(uint8_t _) { REG_RED = REG_BLUE = REG_GREEN = 0xFF; }
void purple(uint8_t _) {
  REG_RED = 0x80;
  REG_GREEN = 0;
  REG_BLUE = 0x80;
}
void aqua(uint8_t _) {
  REG_RED = 0;
  REG_GREEN = 0xFF;
  REG_BLUE = 0xFF;
}
void red(uint8_t _) {
  REG_RED = 0xFF;
  REG_GREEN = 0;
  REG_BLUE = 0;
}

const rgb_function rgb_functions[] = {rainbow, white, purple, aqua, red};

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

int main() {
  init_timers();
  init_switch();

  bool switch_flag_set = false, switch_pressed_tmp = false;
  uint8_t switch_cnt = 0, current_func = 0;
  uint16_t anim_step = 0;

  while (1) {
    if (switch_pressed_tmp != SWITCH_PRESSED) {
      switch_pressed_tmp = SWITCH_PRESSED;
      switch_cnt = 0;
    } else if (switch_cnt > DEBOUNCE_TIME_MS / LOOP_PERIOD_MS) {
      if (switch_pressed_tmp && !switch_flag_set) {
        if (++current_func >= ARRAY_LEN(rgb_functions)) {
          current_func = 0;
        }
        switch_flag_set = true;
      } else if (!switch_pressed_tmp) {
        switch_flag_set = false;
      }
    } else {
      ++switch_cnt;
    }

    anim_step += ADVANCE_PER_LOOP;

    rgb_functions[current_func](anim_step / UINT8_MAX);

    _delay_ms(LOOP_PERIOD_MS);
  }
}
