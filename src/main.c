#include "renderer.h"
#include "clock.h"
#include "input.h"

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

	f32 camera_sensetivity = 1e-1f;
	f32 camera_speed = 10.f;
	camera camera = Camera(Vec3(0.f, 0.f, 0.f), 300.f/400.f, 3.14159f/4.f, 0.f, 0.f, 1.f, 1e5f);
	setCamera(camera);

	quat orientation = Quat(Vec3(1.f, 0.f, 0.f), 0.f);

	initInput();
	setMouseRelative(true);
	initClock();
	while (isRendererOpen())
	{
		processEvents();

		// Update
		f32 delta_time = getDeltaTime();
		quat rot1 = Quat(Vec3(1.f, 0.f, 0.f), 0*1.f*delta_time);
		quat rot2 = Quat(Vec3(0.f, 1.f, 0.f), 0*0.5f*delta_time);
		quat rotation = quatMultiply(rot1, rot2);
		orientation = quatNormalize(quatMultiply(orientation, rotation));
		mat4 transform = mat4Multiply(TranslationTransform(0.f, 0.f, 15.f), mat4Multiply(RotationTransform(orientation), ScaleTransform(2.f, 2.f, 2.f)));

		if (isKeyDown("escape"))
		{
			break;
		}
		vec3 velocity = Vec3(0.f, 0.f, 0.f);
		if (isKeyDown("W"))
		{
			velocity.z += 1.f;
		}
		if (isKeyDown("A"))
		{
			velocity.x -= 1.f;
		}
		if (isKeyDown("S"))
		{
			velocity.z -= 1.f;
		}
		if (isKeyDown("D"))
		{
			velocity.x += 1.f;
		}
		if (isKeyDown("Q"))
		{
			velocity.y -= 1.f;
		}
		if (isKeyDown("E"))
		{
			velocity.y += 1.f;
		}
		if (velocity.x != 0.f || velocity.y != 0.f || velocity.z != 0.f)
		{
			velocity = vec3Scale(camera_speed, vec3Normalize(velocity));
			velocity = vec3RotateByQuat(velocity, Quat(Vec3(0.f, -1.f, 0.f), camera.yaw));
		}
		camera.position = vec3Add(camera.position, vec3Scale(delta_time, velocity));
		vec2 camera_movement = getMouseRelative();
		camera.yaw -= camera_sensetivity*camera_movement.x*delta_time;
		camera.pitch -= camera_sensetivity*camera_movement.y*delta_time;
		if (isKeyDown("R"))
		{
			camera.position = Vec3(0.f, 0.f, 0.f);
			camera.yaw = camera.pitch = 0.f;
		}
		setCamera(camera);
		
		clearBuffer(Color(0, 0, 0, 255));
		// drawFlatObj(obj, transform, Color(255, 0, 0, 255));
		drawTexturedObj(obj, transform, text);
		swapBuffers();
	}

	destroyObj(obj);
	destroyTexture(text);
	closeRenderer();
	return EXIT_SUCCESS;
}
