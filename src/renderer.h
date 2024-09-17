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

	mat4 camera_transform;
	mat4 view_transform;
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

// All triangles drawn must have clockwise winding
void rasterizeFlatTriangle(vec4 p1, vec4 p2, vec4 p3, color color);

#endif // RENDERER_H
