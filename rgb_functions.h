#include <inttypes.h>


#ifndef _RGB_FUNCTIONS_H_
#define _RGB_FUNCTIONS_H_

#define GET_RED(rgb) ((uint8_t)((rgb >> 16) & 0xFF));
#define GET_GREEN(rgb) ((uint8_t)((rgb >> 8) & 0xFF));
#define GET_BLUE(rgb) ((uint8_t)(rgb & 0xFF));

#define VIOLET 0x9400D3
#define INDIGO 0x4B0082
#define BLUE 0x0000FF
#define GREEN 0x00FF00
#define YELLOW 0xFFFF00
#define ORANGE 0xFF7F00
#define RED 0xFF0000
#define BLACK 0;

extern const uint32_t COLORS[7];

uint32_t rainbow(uint8_t step);
uint32_t random_value(uint8_t step);
uint32_t fixed_color(uint32_t color, uint32_t counter_value);

#endif