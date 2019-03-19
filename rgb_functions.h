#ifndef _RGB_FUNCTIONS_H_
#define _RGB_FUNCTIONS_H_

#include <inttypes.h>

#define RAINBOW_BLEND_FACTOR 200

typedef uint32_t rgb_value_t;

uint8_t rgb_red(rgb_value_t rgb);
uint8_t rgb_green(rgb_value_t rgb);
uint8_t rgb_blue(rgb_value_t rgb);

const rgb_value_t COLORS[7];

rgb_value_t off(uint8_t _);

rgb_value_t rainbow(uint8_t step);
rgb_value_t random_value(uint8_t step);

rgb_value_t red(uint8_t _);
rgb_value_t green(uint8_t _);
rgb_value_t blue(uint8_t _);

#endif