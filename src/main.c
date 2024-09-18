#include "renderer.h"

int main(void)
{
	if (!initRenderer("Hello", 400, 300, 4))
	{
		return EXIT_FAILURE;
	}

	Texture text = loadTexture("res/image.bmp");
	if (!text.pixels)
	{
		puts("Couldn't load texture!");
	}

	Obj obj = loadObj("res/cube.obj");
	if (!obj.triangles)
	{
		puts("Couldn't load object!");
	}

	camera camera = Camera(Vec3(0.f, 0.f, 0.f), 300.f/400.f, 3.14159f/4.f, 0.f, 0.f, 1.f, 1e5f);
	setCamera(camera);

	mat4 transform = mat4Multiply(TranslationTransform(0.f, 0.f, 15.f), ScaleTransform(2.f, 2.f, 2.f));

	while (isRendererOpen())
	{
		processEvents();

		clearBuffer(Color(0, 0, 0, 255));
		drawFlatObj(obj, transform, Color(255, 0, 0, 255));
		swapBuffers();
	}

	destroyObj(obj);
	destroyTexture(text);
	closeRenderer();
	return EXIT_SUCCESS;
}
