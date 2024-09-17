#include "renderer.h"

static Renderer gRenderer;

bool initRenderer(const char *title, i32 width, i32 height, i32 scale)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		SDL_Quit();
		return false;
	}

	gRenderer.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, scale*width, scale*height, 0);
	if (!gRenderer.window)
	{
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	
	gRenderer.renderer = SDL_CreateRenderer(gRenderer.window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
	if (!gRenderer.renderer)
	{
		SDL_DestroyWindow(gRenderer.window);
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	gRenderer.texture = SDL_CreateTexture(gRenderer.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!gRenderer.texture)
	{
		SDL_DestroyRenderer(gRenderer.renderer);
		SDL_DestroyWindow(gRenderer.window);
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	gRenderer.frame_buffer = malloc(width*height*sizeof(*gRenderer.frame_buffer));
	if (!gRenderer.frame_buffer)
	{
		SDL_DestroyTexture(gRenderer.texture);
		SDL_DestroyRenderer(gRenderer.renderer);
		SDL_DestroyWindow(gRenderer.window);
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	gRenderer.z_buffer = malloc(width*height*sizeof(*gRenderer.z_buffer));
	if (!gRenderer.z_buffer)
	{
		free(gRenderer.z_buffer);
		SDL_DestroyTexture(gRenderer.texture);
		SDL_DestroyRenderer(gRenderer.renderer);
		SDL_DestroyWindow(gRenderer.window);
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	gRenderer.width = width;
	gRenderer.height = height;
	gRenderer.open = true;

	return true;
}

void closeRenderer(void)
{
	free(gRenderer.frame_buffer);
	free(gRenderer.z_buffer);
	SDL_DestroyTexture(gRenderer.texture);
	SDL_DestroyRenderer(gRenderer.renderer);
	SDL_DestroyWindow(gRenderer.window);
	IMG_Quit();
	SDL_Quit();
}

void clearBuffer(color color)
{
	for (i32 i = 0; i < gRenderer.width*gRenderer.height; i++)
	{
		gRenderer.frame_buffer[i] = color;
		gRenderer.z_buffer[i] = INFINITY;
	}
}

void swapBuffers(void)
{
	SDL_UpdateTexture(gRenderer.texture, NULL, gRenderer.frame_buffer, gRenderer.width*sizeof(*gRenderer.frame_buffer));
	SDL_RenderCopy(gRenderer.renderer, gRenderer.texture, NULL, NULL);
	SDL_RenderPresent(gRenderer.renderer);
}

bool isRendererOpen(void)
{
	return gRenderer.open;
}

void processEvents(void)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			gRenderer.open = false;
			break;
		}
	}
}

void setPixel(i32 x, i32 y, color color, f32 z)
{
	i32 index = y*gRenderer.width + x;
	if (x < 0 || x >= gRenderer.width || y < 0 || y >= gRenderer.height || z > gRenderer.z_buffer[index])
	{
		return;
	}
	gRenderer.frame_buffer[index] = color;
	gRenderer.z_buffer[index] = z;
}

void blendPixel(i32 x, i32 y, color color, f32 z)
{
	i32 index = y*gRenderer.width + x;
	if (x < 0 || x >= gRenderer.width || y < 0 || y >= gRenderer.height || z > gRenderer.z_buffer[index])
	{
		return;
	}
	gRenderer.frame_buffer[index] = blendColors(gRenderer.frame_buffer[index], color);
	gRenderer.z_buffer[index] = z;
}

void setCamera(camera camera)
{
	// TODO
	(void)camera;
}
