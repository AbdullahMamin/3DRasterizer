#ifndef INPUT_H
#define INPUT_H
#include <SDL2/SDL.h>
#include "vector.h"

void initInput(void);
bool isKeyDown(const char *key_name);
void setMouseRelative(bool set);
vec2 getMouseRelative(void);

#endif // INPUT_H
