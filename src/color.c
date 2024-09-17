#include "color.h"

inline color Color(u8 r, u8 g, u8 b, u8 a)
{
	return (color){r, g, b, a};
}

inline color blendColors(color background, color foreground)
{
	f32 alpha = (f32)foreground.a/255.f;
	u8 r = clamp((1.f - alpha)*background.r + alpha*foreground.r, 0, 255);
	u8 g = clamp((1.f - alpha)*background.g + alpha*foreground.g, 0, 255);
	u8 b = clamp((1.f - alpha)*background.b + alpha*foreground.b, 0, 255);
	return (color){r, g, b, 255};
}
