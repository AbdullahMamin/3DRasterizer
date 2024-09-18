#include "input.h"

static const u8 *gKeyboardState;
static i32 gN_Keys;

void initInput(void)
{
	gKeyboardState = SDL_GetKeyboardState(&gN_Keys);
}

bool isKeyDown(const char *key_name)
{
	SDL_Scancode sc = SDL_GetScancodeFromName(key_name);
	if (sc >= (u32)gN_Keys)
	{
		return false;
	}
	return gKeyboardState[sc];
}

void setMouseRelative(bool set)
{
	SDL_SetRelativeMouseMode(set);
}

vec2 getMouseRelative(void)
{
	i32 x, y;
	SDL_GetRelativeMouseState(&x, &y);
	return Vec2(x, y);
}
