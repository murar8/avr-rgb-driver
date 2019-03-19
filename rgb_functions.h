#ifndef _RGB_FUNCTIONS_H_
#define _RGB_FUNCTIONS_H_

#include <inttypes.h>

#define RGB_BLEND_FACTOR 200

typedef struct rgb_state_t {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} RGB_state;

typedef uint8_t *(*rgb_function)(uint8_t);

const rgb_function RGB_FUNCTIONS_MODE0[] = {};
const rgb_function RGB_FUNCTIONS_MODE1[] = {};

#endif