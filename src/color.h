#ifndef COLOR_H
#define COLOR_H
#include "types.h"
#include "util.h"

typedef struct
{
	u8 r, g, b, a;
} color;

color Color(u8 r, u8 g, u8 b, u8 a);
color blendColors(color background, color foreground);

#endif // COLOR_H
