#include "renderer.h"
#include "clock.h"

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


	quat orientation = Quat(Vec3(1.f, 0.f, 0.f), 0.f);
	
	initClock();
	while (isRendererOpen())
	{
		processEvents();

		// Update
		f32 delta_time = getDeltaTime();
		quat rot1 = Quat(Vec3(1.f, 0.f, 0.f), 1.f*delta_time);
		quat rot2 = Quat(Vec3(0.f, 1.f, 0.f), 0.5f*delta_time);
		quat rotation = quatMultiply(rot1, rot2);
		orientation = quatNormalize(quatMultiply(orientation, rotation));
		mat4 transform = mat4Multiply(TranslationTransform(0.f, 0.f, 15.f), mat4Multiply(RotationTransform(orientation), ScaleTransform(2.f, 2.f, 2.f)));
		

		clearBuffer(Color(0, 0, 0, 255));
		drawFlatObj(obj, transform, Color(255, 0, 0, 255));
		swapBuffers();
	}

	destroyObj(obj);
	destroyTexture(text);
	closeRenderer();
	return EXIT_SUCCESS;
}
