#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "transform.h"
#include "color.h"

typedef struct
{
	bool open;
	i32 width, height;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	
	color *frame_buffer;
	f32 *z_buffer;
} Renderer;

bool initRenderer(const char *title, i32 width, i32 height, i32 scale);
void closeRenderer(void);
void clearBuffer(color color);
void swapBuffers(void);
bool isRendererOpen(void);
void processEvents(void);

void setPixel(i32 x, i32 y, color color, f32 z);
void blendPixel(i32 x, i32 y, color color, f32 z);
void setCamera(camera camera);

#endif // RENDERER_H
