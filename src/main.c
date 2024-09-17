#include "renderer.h"

int main(void)
{
	if (!initRenderer("Hello", 400, 300, 4))
	{
		return EXIT_FAILURE;
	}

	while (isRendererOpen())
	{
		processEvents();

		clearBuffer(Color(255, 0, 0, 255));
		for (i32 i = 0; i < 100; i ++)
		{
			for (i32 j = 0; j < 100; j++)
			{
				blendPixel(i, j, Color(0, 255, 0, clamp(10000.f/(i*j), 0, 255)), 1.f);
			}
		}
		swapBuffers();
	}

	return EXIT_SUCCESS;
}
