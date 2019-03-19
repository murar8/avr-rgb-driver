#ifndef _RGB_FUNCTIONS_H_
#define _RGB_FUNCTIONS_H_

#include <inttypes.h>

#define RGB_BLEND_FACTOR 200
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

typedef struct rgb_state_t {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} RGB_state;

typedef RGB_state (*rgb_function)(uint8_t);

RGB_state off(uint8_t _);

RGB_state rainbow(uint8_t step);

RGB_state red(uint8_t _);
RGB_state green(uint8_t _);
RGB_state blue(uint8_t _);

const rgb_function RGB_FUNCTIONS_MODE0[] = {off, rainbow};
const rgb_function RGB_FUNCTIONS_MODE1[] = {off, red, green, blue};

#endif