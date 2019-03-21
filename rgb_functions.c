#include "rgb_functions.h"
#include <stdlib.h>
#include "config.h"

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))
#define TO_RGB(r, g, b) (((uint32_t)r << 16) + ((uint16_t)g << 8) + b)

const uint32_t COLORS[] = {VIOLET, INDIGO, BLUE, GREEN, YELLOW, ORANGE, RED};

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

uint8_t sine_shifted(uint8_t step, uint8_t blend_factor) {
  if (step > blend_factor) return 0;
  return sine_uint8[((uint16_t)step * UINT8_MAX) / blend_factor];
}

uint32_t rainbow(uint8_t step) {
  uint8_t red, green, blue;
  red = sine_shifted(step, RAINBOW_BLEND_FACTOR);
  green = sine_shifted((step + 85) % 255, RAINBOW_BLEND_FACTOR);
  blue = sine_shifted((step + 170) % 255, RAINBOW_BLEND_FACTOR);
  return TO_RGB(red, green, blue);
}

uint32_t random_value(uint8_t step) {
  static uint32_t old_color = 0, rand_color = 0;

  /*if (step == 0) {
    uint8_t rand_index = ((uint64_t)rand() * ARRAY_LEN(COLORS)) / RAND_MAX;
    old_color = rand_color;
    rand_color = COLORS[rand_index];
  }

  uint32_t weighted_old =
      ((uint64_t)old_color * (UINT8_MAX - step)) / UINT8_MAX;
  uint32_t weighted_rand = ((uint64_t)rand_color * step) / UINT8_MAX;
  uint32_t current_color = (weighted_old + weighted_rand);*/

  return rand_color;
}

uint32_t fixed_color(uint32_t color, uint32_t counter_value) {
  static uint32_t counter_start = 0, last_color = 0;
  static uint8_t in_transition = 0;

  if (color != last_color) {
    if (!in_transition) {
      counter_start = counter_value;
      in_transition = 1;
    }
    if (counter_value - counter_start > TRANSITION_DELTA) {
      last_color = color;
      in_transition = 0;
    }
    uint8_t red, green, blue;
    red = GET_RED()
  }
  return color;
}
