#include "renderer.h"

static Renderer gRenderer;

Texture loadTexture(const char *file_path)
{
	Texture texture = (Texture){0, 0, NULL};
	u32 data_offset;
	u16 bit_depth;
	FILE *file = fopen(file_path, "rb");
	if (!file)
	{
		return texture;
	}

	if (fgetc(file) != 'B' || fgetc(file) != 'M')
	{
		fclose(file);
		return texture;
	}

	fseek(file, 0x0A, SEEK_SET);
	if (fread(&data_offset, sizeof(data_offset), 1, file) != 1)
	{
		fclose(file);
		return texture;
	}

	fseek(file, 0x12, SEEK_SET);
	if (fread(&texture.width, sizeof(texture.width), 1, file) != 1)
	{
		fclose(file);
		return texture;
	}

	fseek(file, 0x16, SEEK_SET);
	if (fread(&texture.height, sizeof(texture.height), 1, file) != 1)
	{
		fclose(file);
		return texture;
	}

	fseek(file, 0x1C, SEEK_SET);
	if (fread(&bit_depth, sizeof(bit_depth), 1, file) != 1)
	{
		fclose(file);
		return texture;
	}
	if (bit_depth != 32)
	{
		printf("BMP incorrect depth: got %u instead of 32\n", bit_depth);
		fclose(file);
		return texture;
	}

	texture.pixels = malloc(texture.width*texture.height*sizeof(*texture.pixels));
	if (!texture.pixels)
	{
		fclose(file);
		return texture;
	}

	fseek(file, data_offset, SEEK_SET);
	for (i32 i = texture.height - 1; i >= 0; i--)
	{
		if (fread(texture.pixels + texture.width*i, sizeof(*texture.pixels), texture.width, file) != texture.width)
		{
			free(texture.pixels);
			texture.pixels = NULL;
			fclose(file);
			return texture;
		}
	}

	for (i32 i = 0; i < (i32)texture.width*(i32)texture.height; i++)
	{
		color old = texture.pixels[i];
		texture.pixels[i] = Color(old.b, old.g, old.r, old.a);
	}

	fclose(file);
	return texture;
}

void destroyTexture(Texture texture)
{
	free(texture.pixels);
}

color getTexel(const Texture texture, vec2 uv)
{
	i32 x = clamp(uv.x*(texture.width - 1), 0, texture.width - 1);
	i32 y = clamp(uv.y*(texture.height - 1), 0, texture.height - 1);
	return texture.pixels[y*texture.width + x];
}

TriangleStack createTriangleStack(i32 capacity)
{
	TriangleStack stack;
	stack.triangles = malloc(capacity*sizeof(*stack.triangles));
	if (!stack.triangles)
	{
		return stack;
	}

	stack.capacity = capacity;
	stack.n_triangles = 0;
	return stack;
}

bool enlargeTriangleStack(TriangleStack *stack, i32 amount)
{
	Triangle *new_triangles = realloc(stack->triangles, (stack->capacity + amount)*sizeof(*new_triangles));
	if (!new_triangles)
	{
		return false;
	}

	stack->triangles = new_triangles;
	stack->capacity += amount;
	return true;
}

void destroyTriangleStack(TriangleStack *stack)
{
	free(stack->triangles);
}

bool isTriangleStackFull(const TriangleStack *stack)
{
	return stack->n_triangles == stack->capacity;
}

bool isTriangleStackEmpty(const TriangleStack *stack)
{
	return stack->n_triangles == 0;
}

void pushTriangleStack(TriangleStack *stack, Triangle triangle)
{
	assert(!isTriangleStackFull(stack));
	stack->triangles[stack->n_triangles++] = triangle;
}

Triangle popTriangleStack(TriangleStack *stack)
{
	assert(!isTriangleStackEmpty(stack));
	return stack->triangles[--stack->n_triangles];
}

i32 clipTriangle(Triangle *destination, Triangle triangle_to_be_clipped, plane clipping_plane)
{
	i32 inside_count = 0;
	i32 outside_count = 0;
	vec3 inside_points[3];
	vec2 inside_uvs[3];
	vec3 outside_points[3];
	vec2 outside_uvs[3];
	vec3 temp;

	temp = Vec4ToVec3(triangle_to_be_clipped.p1);
	if (isPointInPlane(temp, clipping_plane))
	{
		inside_points[inside_count] = temp;
		inside_uvs[inside_count++] = triangle_to_be_clipped.uv1;
	}
	else
	{
		outside_points[outside_count] = temp;
		outside_uvs[outside_count++] = triangle_to_be_clipped.uv1;
	}
	temp = Vec4ToVec3(triangle_to_be_clipped.p2);
	if (isPointInPlane(temp, clipping_plane))
	{
		inside_points[inside_count] = temp;
		inside_uvs[inside_count++] = triangle_to_be_clipped.uv2;
	}
	else
	{
		outside_points[outside_count] = temp;
		outside_uvs[outside_count++] = triangle_to_be_clipped.uv2;
	}
	temp = Vec4ToVec3(triangle_to_be_clipped.p3);
	if (isPointInPlane(temp, clipping_plane))
	{
		inside_points[inside_count] = temp;
		inside_uvs[inside_count++] = triangle_to_be_clipped.uv3;
	}
	else
	{
		outside_points[outside_count] = temp;
		outside_uvs[outside_count++] = triangle_to_be_clipped.uv3;
	}

	if (inside_count == 0)
	{
		return 0;
	}

	if (inside_count == 3)
	{
		destination[0] = triangle_to_be_clipped;
		return 1;
	}

	if (inside_count == 1)
	{
		Triangle tri;
		f32 t1 = rayVsPlane(inside_points[0], outside_points[0], clipping_plane);
		f32 t2 = rayVsPlane(inside_points[0], outside_points[1], clipping_plane);

		tri.p1 = Vec3ToVec4(inside_points[0]);
		tri.uv1 = inside_uvs[0];
		tri.p2 = Vec3ToVec4(vec3Add(inside_points[0], vec3Scale(t1, vec3Subtract(outside_points[0], inside_points[0]))));
		tri.uv2 = vec2Add(inside_uvs[0], vec2Scale(t1, vec2Subtract(outside_uvs[0], inside_uvs[0])));
		tri.p3 = Vec3ToVec4(vec3Add(inside_points[0], vec3Scale(t2, vec3Subtract(outside_points[1], inside_points[0]))));
		tri.uv3 = vec2Add(inside_uvs[0], vec2Scale(t2, vec2Subtract(outside_uvs[1], inside_uvs[0])));

		destination[0] = tri;
		return 1;
	}

	// if (inside_count == 2)

	Triangle tri1, tri2;
	f32 t1 = rayVsPlane(inside_points[0], outside_points[0], clipping_plane);
	f32 t2 = rayVsPlane(inside_points[1], outside_points[0], clipping_plane);

	vec4 common_p = Vec3ToVec4(vec3Add(inside_points[0], vec3Scale(t1, vec3Subtract(outside_points[0], inside_points[0]))));
	vec2 common_uv = vec2Add(inside_uvs[0], vec2Scale(t1, vec2Subtract(outside_uvs[0], inside_uvs[0])));

	tri1.p1 = Vec3ToVec4(inside_points[0]);
	tri1.uv1 = inside_uvs[0];
	tri1.p2 = Vec3ToVec4(inside_points[1]);
	tri1.uv2 = inside_uvs[1];
	tri1.p3 = common_p;
	tri1.uv3 = common_uv;

	tri2.p1 = Vec3ToVec4(inside_points[1]);
	tri2.uv1 = inside_uvs[1];
	tri2.p2 = Vec3ToVec4(vec3Add(inside_points[1], vec3Scale(t2, vec3Subtract(outside_points[0], inside_points[1]))));
	tri2.uv2 = vec2Add(inside_uvs[1], vec2Scale(t2, vec2Subtract(outside_uvs[0], inside_uvs[1])));
	tri2.p3 = common_p;
	tri2.uv3 = common_uv;
	
	destination[0] = tri1;
	destination[1] = tri2;
	return 2;
}

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
		free(gRenderer.frame_buffer);
		SDL_DestroyTexture(gRenderer.texture);
		SDL_DestroyRenderer(gRenderer.renderer);
		SDL_DestroyWindow(gRenderer.window);
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	gRenderer.front_stack = createTriangleStack(256);
	if (!gRenderer.front_stack.triangles)
	{
		free(gRenderer.z_buffer);
		free(gRenderer.frame_buffer);
		SDL_DestroyTexture(gRenderer.texture);
		SDL_DestroyRenderer(gRenderer.renderer);
		SDL_DestroyWindow(gRenderer.window);
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	gRenderer.back_stack = createTriangleStack(256);
	if (!gRenderer.back_stack.triangles)
	{
		destroyTriangleStack(&gRenderer.front_stack);
		free(gRenderer.z_buffer);
		free(gRenderer.frame_buffer);
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
	destroyTriangleStack(&gRenderer.back_stack);
	destroyTriangleStack(&gRenderer.front_stack);
	free(gRenderer.z_buffer);
	free(gRenderer.frame_buffer);
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
	// TODO set clipping planes
	gRenderer.camera_transform = CameraTransform(camera);
	gRenderer.view_transform = CameraViewTransform(camera);
}

void rasterizeFlatTriangle(vec4 p1, vec4 p2, vec4 p3, color color)
{
	// TODO optimize
	f32 min_x = min(min(p1.x, p2.x), p3.x);
	f32 max_x = max(max(p1.x, p2.x), p3.x);
	f32 min_y = min(min(p1.y, p2.y), p3.y);
	f32 max_y = max(max(p1.y, p2.y), p3.y);
	vec2 p1_small = Vec2(p1.x, p1.y);
	vec2 p2_small = Vec2(p2.x, p2.y);
	vec2 p3_small = Vec2(p3.x, p3.y);
	vec2 p1_p2 = vec2Subtract(p2_small, p1_small);
	vec2 p2_p3 = vec2Subtract(p3_small, p2_small);
	vec2 p3_p1 = vec2Subtract(p1_small, p3_small);
	for (i32 x = min_x; x <= max_x; x++)
	{
		for (i32 y = min_y; y <= max_y; y++)
		{
			vec2 p = Vec2(x, y);
			vec2 p1_p = vec2Subtract(p, p1_small);
			vec2 p2_p = vec2Subtract(p, p2_small);
			vec2 p3_p = vec2Subtract(p, p3_small);
			if (vec2Cross(p1_p, p1_p2) <= 0.f && vec2Cross(p2_p, p2_p3) <= 0.f && vec2Cross(p3_p, p3_p1) <= 0.f)
			{
				setPixel(x, y, color, 1.f);
			}
		}
	}
}

void rasterizeTexturedTriangle(vec4 p1, vec4 p2, vec4 p3, vec2 uv1, vec2 uv2, vec2 uv3, const Texture texture)
{
	// TODO optimize
	f32 min_x = min(min(p1.x, p2.x), p3.x);
	f32 max_x = max(max(p1.x, p2.x), p3.x);
	f32 min_y = min(min(p1.y, p2.y), p3.y);
	f32 max_y = max(max(p1.y, p2.y), p3.y);
	vec2 p1_small = Vec2(p1.x, p1.y);
	vec2 p2_small = Vec2(p2.x, p2.y);
	vec2 p3_small = Vec2(p3.x, p3.y);
	vec2 p1_p2 = vec2Subtract(p2_small, p1_small);
	vec2 p2_p3 = vec2Subtract(p3_small, p2_small);
	vec2 p3_p1 = vec2Subtract(p1_small, p3_small);
	for (i32 x = min_x; x <= max_x; x++)
	{
		for (i32 y = min_y; y <= max_y; y++)
		{
			vec2 p = Vec2(x, y);
			vec2 p1_p = vec2Subtract(p, p1_small);
			vec2 p2_p = vec2Subtract(p, p2_small);
			vec2 p3_p = vec2Subtract(p, p3_small);
			if (vec2Cross(p1_p, p1_p2) <= 0.f && vec2Cross(p2_p, p2_p3) <= 0.f && vec2Cross(p3_p, p3_p1) <= 0.f)
			{
				f32 w1 = vec2Cross(p2_p, p3_p);
				f32 w2 = vec2Cross(p3_p, p1_p);
				f32 w3 = vec2Cross(p1_p, p2_p);
				f32 total = w1 + w2 + w3;
				w1 /= total;
				w2 /= total;
				w3 /= total;
				setPixel(x, y, getTexel(texture, vec2Add(vec2Add(vec2Scale(w1, uv1), vec2Scale(w2, uv2)), vec2Scale(w3, uv3))), 1.f);
			}
		}
	}
}
