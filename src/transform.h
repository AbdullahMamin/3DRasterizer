#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "vector.h"

typedef struct
{
	vec3 position;
	f32 aspect_ratio; // (height/width)
	f32 horizontal_fov;
	f32 pitch, yaw;
	f32 z_near, z_far;
} camera;

camera Camera(vec3 position, f32 aspect_ratio, f32 horizontal_fov, f32 pitch, f32 yaw, f32 z_near, f32 z_far);

mat4 CameraTransform(camera camera);
mat4 CameraViewTransform(camera camera);
mat4 ScaleTransform(f32 x, f32 y, f32 z);
mat4 TranslationTransform(f32 x, f32 y, f32 z);
mat4 RotationTransform(quat q);

vec4 vec4PerspectiveDivide(vec4 v);
Triangle perspectiveDivideTriangle(Triangle triangle);
Triangle transformTriangle(Triangle triangle, mat4 transform);

#endif // TRANSFORM_H
