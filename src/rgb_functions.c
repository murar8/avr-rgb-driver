#include "rgb_functions.h"
#include <stdlib.h>
#include "config.h"

#define RAINBOW_ANIM_DELTA ((RAINBOW_PERIOD_MS * 1000UL) / TIMER_PERIOD)
#define TRANS_ANIM_DELTA ((COLOR_TRANSITION_PERIOD_MS * 1000UL) / TIMER_PERIOD)
#define COLOR_SWAP_DELTA ((COLOR_SWAP_PERIOD_MS * 1000UL) / TIMER_PERIOD)

#define TO_RGB(r, g, b) (((uint32_t)r << 16) + ((uint16_t)g << 8) + b)

const uint32_t COLORS[] = {RED, MAGENTA, VIOLET, BLUE,
                           CYAN, GREEN, YELLOW, DARK_ORANGE};

const uint8_t COLORS_LEN = sizeof(COLORS) / sizeof(uint32_t);

const uint8_t sine_uint8[] = {
    0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 9,
    10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33,
    35, 37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70,
    73, 76, 79, 82, 85, 88, 90, 93, 97, 100, 103, 106, 109, 112, 115,
    118, 121, 124, 128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162,
    165, 167, 170, 173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203,
    206, 208, 211, 213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235,
    237, 238, 240, 241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252, 253,
    253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253,
    253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 238, 237,
    235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 215, 213, 211, 208, 206,
    203, 201, 198, 196, 193, 190, 188, 185, 182, 179, 176, 173, 170, 167, 165,
    162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131, 128, 124, 121, 118,
    115, 112, 109, 106, 103, 100, 97, 93, 90, 88, 85, 82, 79, 76, 73,
    70, 67, 65, 62, 59, 57, 54, 52, 49, 47, 44, 42, 40, 37, 35,
    33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11, 10,
    9, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0,
    0};

uint8_t rainbow_half_sine(uint8_t step)
{
  if (step > RAINBOW_BLEND_FACTOR)
    return 0;
  return sine_uint8[((uint16_t)step * UINT8_MAX) / RAINBOW_BLEND_FACTOR];
}

uint8_t interpolate_uint8(uint8_t a, uint8_t b, uint8_t weight)
{
  return ((b - a) * weight / UINT8_MAX) + a;
}

uint32_t rgb_interpolation(uint32_t color_start, uint32_t color_end,
                           uint8_t step)
{
  uint8_t red, green, blue;
  red = interpolate_uint8(GET_RED(color_start), GET_RED(color_end), step);
  green = interpolate_uint8(GET_GREEN(color_start), GET_GREEN(color_end), step);
  blue = interpolate_uint8(GET_BLUE(color_start), GET_BLUE(color_end), step);

  return TO_RGB(red, green, blue);
}

uint32_t rainbow(uint32_t counter_value)
{
  uint8_t step =
      ((counter_value % RAINBOW_ANIM_DELTA) * UINT8_MAX) / RAINBOW_ANIM_DELTA;
  uint8_t red, green, blue;
  red = rainbow_half_sine(step);
  green = rainbow_half_sine(step + 85);
  blue = rainbow_half_sine(step + 170);
  return TO_RGB(red, green, blue);
}

uint32_t slow_changing(uint32_t counter_value)
{
  static uint32_t old_color;
  uint32_t color_index = (counter_value / COLOR_SWAP_DELTA) % COLORS_LEN;
  uint32_t color = COLORS[color_index];
  uint32_t step = counter_value % COLOR_SWAP_DELTA;
  if (step < TRANS_ANIM_DELTA)
  {
    return rgb_interpolation(old_color, color,
                             UINT8_MAX * step / TRANS_ANIM_DELTA);
  }
  else
  {
    old_color = color;
    return color;
  }
}

uint32_t fixed_color(uint32_t color, uint32_t counter_value)
{
  static uint32_t counter_start = 0, color_last = 0;
  static uint8_t in_transition = 0;

  if (color != color_last)
  {
    if (!in_transition)
    {
      counter_start = counter_value;
      in_transition = 1;
    }
    uint32_t delta = counter_value - counter_start;
    if (delta > TRANS_ANIM_DELTA)
    {
      color_last = color;
      in_transition = 0;
    }

    uint8_t step = (uint32_t)UINT8_MAX * delta / TRANS_ANIM_DELTA;
    return rgb_interpolation(color_last, color, step);
  }
  return color;
}
