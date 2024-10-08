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

Obj loadObj(const char *file_path)
{
	Obj obj = (Obj){0, NULL};
	FILE *file = fopen(file_path, "r");
	if (!file)
	{
		return obj;
	}

	i32 i_triangles = 0;
	i32 i_vertices = 0;
	i32 n_vertices = 0;
	i32 i_uvs = 0;
	i32 n_uvs = 0;
	vec4 *vertices;
	vec2 *uvs;

	char *line = NULL;
	size n = 0;
	while (getline(&line, &n, file) != -1)
	{
		if (line[0] == 'v' && line[1] == ' ')
		{
			// vertex coordinate
			n_vertices++;
		}
		else if (line[0] == 'v' && line[1] == 't')
		{
			// texture coordinate
			n_uvs++;
		}
		else if (line[0] == 'f' && line[1] == ' ')
		{
			// face element (triangle)
			obj.n_triangles++;
		}

		free(line);
		line = NULL;
		n = 0;
	}

	vertices = malloc(n_vertices*sizeof(*vertices));
	if (!vertices)
	{
		fclose(file);
		return obj;
	}

	uvs = malloc(n_uvs*sizeof(*uvs));
	if (!uvs)
	{
		free(vertices);
		fclose(file);
		return obj;
	}

	obj.triangles = malloc(obj.n_triangles*sizeof(*obj.triangles));
	if (!obj.triangles)
	{
		free(uvs);
		free(vertices);
		return obj;
	}

	fseek(file, 0, SEEK_SET);
	line = NULL;
	n = 0;
	while (getline(&line, &n, file) != -1)
	{
		if (line[0] == 'v' && line[1] == ' ')
		{
			// vertex coordinate
			line[0] = ' ';
			sscanf(line, "%f %f %f", &vertices[i_vertices].x, &vertices[i_vertices].y, &vertices[i_vertices].z);
			// printf("%f %f %f\n", vertices[i_vertices].x, vertices[i_vertices].y, vertices[i_vertices].z);
			vertices[i_vertices].w = 1.f;
			i_vertices++;
		}
		else if (line[0] == 'v' && line[1] == 't')
		{
			// texture coordinate
			line[0] = line[1] = ' ';
			sscanf(line, "%f %f", &uvs[i_uvs].x, &uvs[i_uvs].y);
			// printf("%f %f\n", uvs[i_uvs].x, uvs[i_uvs].y);
			i_uvs++;
		}
		else if (line[0] == 'f' && line[1] == ' ')
		{
			// face element (triangle)
			line[0] = ' ';
			for (i32 i = 0; i < (i32)n; i++)
			{
				if (line[i] == '/')
				{
					line[i] = ' ';
				}
			}
			i32 p1, uv1, p2, uv2, p3, uv3;
			sscanf(line, "%d %d %d %d %d %d", &p1, &uv1, &p2, &uv2, &p3, &uv3);
			// printf("%d %d %d %d %d %d\n", p1, uv1, p2, uv2, p3, uv3);
			obj.triangles[i_triangles] = (Triangle){
				vertices[p1 - 1],
				vertices[p2 - 1],
				vertices[p3 - 1],
				uvs[uv1 - 1],
				uvs[uv2 - 1],
				uvs[uv3 - 1],
			};
			i_triangles++;
		}

		free(line);
		line = NULL;
		n = 0;
	}

	free(uvs);
	free(vertices);
	fclose(file);
	return obj;
}

void destroyObj(Obj obj)
{
	free(obj.triangles);
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

void doClippingRound(plane clipping_plane)
{
	while (!isTriangleStackEmpty(&gRenderer.front_stack))
	{
		Triangle clipped_triangles[2];
		i32 n_triangles = clipTriangle(clipped_triangles, popTriangleStack(&gRenderer.front_stack), clipping_plane);
		for (i32 i = 0; i < n_triangles; i++)
		{
			if (isTriangleStackFull(&gRenderer.back_stack))
			{
				enlargeTriangleStack(&gRenderer.back_stack, gRenderer.back_stack.capacity);
			}
			pushTriangleStack(&gRenderer.back_stack, clipped_triangles[i]);
		}
	}
	TriangleStack temp = gRenderer.front_stack;
	gRenderer.front_stack = gRenderer.back_stack;
	gRenderer.back_stack = temp;
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
	f32 sine_horizontal = sinf(camera.horizontal_fov/2.f);
	f32 cosine_horizontal = cosf(camera.horizontal_fov/2.f);
	f32 half_vertical_fov = atanf(camera.aspect_ratio*sine_horizontal/cosine_horizontal);
	f32 sine_vertical = sinf(half_vertical_fov);
	f32 cosine_vertical = cosf(half_vertical_fov);
	gRenderer.clipping_planes[0] = (plane){0.f, 0.f, 1.f, camera.z_near};
	gRenderer.clipping_planes[1] = (plane){0.f, 0.f, -1.f, -camera.z_far};
	gRenderer.clipping_planes[2] = (plane){cosine_horizontal, 0.f, sine_horizontal, 0.f};
	gRenderer.clipping_planes[3] = (plane){-cosine_horizontal, 0.f, sine_horizontal, 0.f};
	gRenderer.clipping_planes[4] = (plane){0.f, cosine_vertical, sine_vertical, 0.f};
	gRenderer.clipping_planes[5] = (plane){0.f, -cosine_vertical, sine_vertical, 0.f};
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
			if ((vec2Cross(p1_p, p1_p2) <= 0.f && vec2Cross(p2_p, p2_p3) <= 0.f && vec2Cross(p3_p, p3_p1) <= 0.f) || (vec2Cross(p1_p, p1_p2) >= 0.f && vec2Cross(p2_p, p2_p3) >= 0.f && vec2Cross(p3_p, p3_p1) >= 0.f))
			{
				f32 w1 = vec2Cross(p2_p, p3_p);
				f32 w2 = vec2Cross(p3_p, p1_p);
				f32 w3 = vec2Cross(p1_p, p2_p);
				f32 total = w1 + w2 + w3;
				w1 /= total;
				w2 /= total;
				w3 /= total;
				f32 inv_z = w1/p1.w + w2/p2.w + w3/p3.w;
				setPixel(x, y, color, 1.f/inv_z);
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
			if ((vec2Cross(p1_p, p1_p2) <= 0.f && vec2Cross(p2_p, p2_p3) <= 0.f && vec2Cross(p3_p, p3_p1) <= 0.f) || (vec2Cross(p1_p, p1_p2) >= 0.f && vec2Cross(p2_p, p2_p3) >= 0.f && vec2Cross(p3_p, p3_p1) >= 0.f))
			{
				f32 w1 = vec2Cross(p2_p, p3_p);
				f32 w2 = vec2Cross(p3_p, p1_p);
				f32 w3 = vec2Cross(p1_p, p2_p);
				f32 total = w1 + w2 + w3;
				w1 /= total;
				w2 /= total;
				w3 /= total;
				f32 inv_z1 = w1/p1.w;
				f32 inv_z2 = w2/p2.w;
				f32 inv_z3 = w3/p3.w;
				f32 z = 1.f/(inv_z1 + inv_z2 + inv_z3);
				setPixel(x, y, getTexel(texture, vec2Scale(z, vec2Add(vec2Add(vec2Scale(inv_z1, uv1), vec2Scale(inv_z2, uv2)), vec2Scale(inv_z3, uv3)))), z);
			}
		}
	}
}

// utility function to transform NDC coordinate into screen coordinate
vec4 ndcToScreen(vec4 p)
{
	// return Vec4((p.x + 1.f)/2.f*(gRenderer.width - 1), (p.y + 1.f)/2.f*(gRenderer.height - 1), p.z, p.w);
	return Vec4((p.x + 0.5f)*(gRenderer.width - 1), (p.y + 0.5f)*(gRenderer.height - 1), p.z, p.w);
}

void drawFlatObj(const Obj obj, mat4 transform, color color)
{
	transform = mat4Multiply(gRenderer.camera_transform, transform);
	for (i32 i = 0; i < obj.n_triangles; i++)
	{
		Triangle triangle = transformTriangle(obj.triangles[i], transform);
		Triangle test = perspectiveDivideTriangle(transformTriangle(triangle, gRenderer.view_transform));
		vec3 p1 = Vec4ToVec3(test.p1);
		vec3 p2 = Vec4ToVec3(test.p2);
		vec3 p3 = Vec4ToVec3(test.p3);
		if (vec3Cross(vec3Subtract(p3, p1), vec3Subtract(p2, p1)).z > 0.f)
		{
			if (isTriangleStackFull(&gRenderer.front_stack))
			{
				enlargeTriangleStack(&gRenderer.front_stack, gRenderer.front_stack.capacity);
			}
			pushTriangleStack(&gRenderer.front_stack, triangle);
		}
	}
	for (i32 i = 0; i < 6; i++)
	{
		doClippingRound(gRenderer.clipping_planes[i]);
	}
	while (!isTriangleStackEmpty(&gRenderer.front_stack))
	{
		Triangle triangle = perspectiveDivideTriangle(transformTriangle(popTriangleStack(&gRenderer.front_stack), gRenderer.view_transform));
		rasterizeFlatTriangle(ndcToScreen(triangle.p1), ndcToScreen(triangle.p2), ndcToScreen(triangle.p3), color);
	}
}

void drawTexturedObj(const Obj obj, mat4 transform, const Texture texture)
{
	transform = mat4Multiply(gRenderer.camera_transform, transform);
	for (i32 i = 0; i < obj.n_triangles; i++)
	{
		Triangle triangle = transformTriangle(obj.triangles[i], transform);
		Triangle test = perspectiveDivideTriangle(transformTriangle(triangle, gRenderer.view_transform));
		vec3 p1 = Vec4ToVec3(test.p1);
		vec3 p2 = Vec4ToVec3(test.p2);
		vec3 p3 = Vec4ToVec3(test.p3);
		if (vec3Cross(vec3Subtract(p3, p1), vec3Subtract(p2, p1)).z > 0.f)
		{
			if (isTriangleStackFull(&gRenderer.front_stack))
			{
				enlargeTriangleStack(&gRenderer.front_stack, gRenderer.front_stack.capacity);
			}
			pushTriangleStack(&gRenderer.front_stack, triangle);
		}
	}
	for (i32 i = 0; i < 6; i++)
	{
		doClippingRound(gRenderer.clipping_planes[i]);
	}
	while (!isTriangleStackEmpty(&gRenderer.front_stack))
	{
		Triangle triangle = perspectiveDivideTriangle(transformTriangle(popTriangleStack(&gRenderer.front_stack), gRenderer.view_transform));
		rasterizeTexturedTriangle(ndcToScreen(triangle.p1), ndcToScreen(triangle.p2), ndcToScreen(triangle.p3), triangle.uv1, triangle.uv2, triangle.uv3, texture);
	}
}
