#include <inttypes.h>

#ifndef _RGB_FUNCTIONS_H_
#define _RGB_FUNCTIONS_H_

#define GET_RED(rgb) (((rgb >> 16) & 0xFF))
#define GET_GREEN(rgb) (((rgb >> 8) & 0xFF))
#define GET_BLUE(rgb) ((rgb & 0xFF))

#define BLACK 0

#define RED 0xFF0000
#define MAGENTA 0xFF00FF
#define VIOLET 0x7F00FF
#define BLUE 0x0000FF
#define CYAN 0x00FFFF
#define GREEN 0x00FF00
#define YELLOW 0xFFFF00
#define DARK_ORANGE 0xFF8000

extern const uint8_t COLORS_LEN;
extern const uint32_t COLORS[];

uint32_t rainbow(uint32_t counter_value);
uint32_t slow_changing(uint32_t counter_value);
uint32_t fixed_color(uint32_t color, uint32_t counter_value);

#endif