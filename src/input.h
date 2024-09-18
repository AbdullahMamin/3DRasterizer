#ifndef INPUT_H
#define INPUT_H
#include <SDL2/SDL.h>
#include "types.h"

void initInput(void);
bool isKeyDown(const char *key_name);

#endif // INPUT_H
