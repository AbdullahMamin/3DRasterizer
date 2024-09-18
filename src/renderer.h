#ifndef RENDERER_H
#define RENDERER_H
#define _GNU_SOURCE
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
	i32 n_triangles;
	Triangle *triangles;
} Obj;

typedef struct
{
	i32 capacity;
	i32 n_triangles;
	Triangle *triangles;
} TriangleStack;

typedef struct
{
	bool open;
	i32 width, height;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	
	color *frame_buffer;
	f32 *z_buffer;

	// Used for clipping
	plane clipping_planes[6]; // near, far, left, right, up, down
	TriangleStack front_stack;
	TriangleStack back_stack;

	mat4 camera_transform;
	mat4 view_transform;
} Renderer;

Texture loadTexture(const char *file_path); // Loads bmp files
void destroyTexture(Texture texture);
color getTexel(const Texture texture, vec2 uv);

Obj loadObj(const char *file_path);
void destroyObj(Obj obj);

TriangleStack createTriangleStack(i32 capacity);
bool enlargeTriangleStack(TriangleStack *stack, i32 amount);
void destroyTriangleStack(TriangleStack *stack);
bool isTriangleStackFull(const TriangleStack *stack);
bool isTriangleStackEmpty(const TriangleStack *stack);
void pushTriangleStack(TriangleStack *stack, Triangle triangle);
Triangle popTriangleStack(TriangleStack *stack);

i32 clipTriangle(Triangle *destination, Triangle triangle_to_be_clipped, plane clipping_plane);

bool initRenderer(const char *title, i32 width, i32 height, i32 scale);
void closeRenderer(void);
void clearBuffer(color color);
void swapBuffers(void);
bool isRendererOpen(void);
void processEvents(void);

void setPixel(i32 x, i32 y, color color, f32 z);
void blendPixel(i32 x, i32 y, color color, f32 z);
void setCamera(camera camera);

void rasterizeFlatTriangle(vec4 p1, vec4 p2, vec4 p3, color color);
void rasterizeTexturedTriangle(vec4 p1, vec4 p2, vec4 p3, vec2 uv1, vec2 uv2, vec2 uv3, const Texture texture);

#endif // RENDERER_H
