#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "transform.h"
#include "color.h"

typedef struct
{
	u32 width, height;
	color *pixels;
} Texture;

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

// Loads bmp files
Texture loadTexture(const char *file_path);
void destroyTexture(Texture texture);
color getTexel(const Texture texture, vec2 uv);

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
void rasterizeTexturedTriangle(vec4 p1, vec4 p2, vec4 p3, vec2 uv1, vec2 uv2, vec2 uv3, const Texture texture);

#endif // RENDERER_H
