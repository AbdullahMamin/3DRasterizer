#include "clock.h"

static u64 gCurrentTicks;

void initClock(void)
{
	gCurrentTicks = SDL_GetTicks64();
}

f32 getDeltaTime(void)
{
	u64 new_ticks = SDL_GetTicks64();
	f32 delta_time = 0.001f*(new_ticks - gCurrentTicks);
	gCurrentTicks = new_ticks;
	return delta_time;
}
