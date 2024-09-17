#include "transform.h"

camera Camera(vec3 position, f32 aspect_ratio, f32 horizontal_fov, f32 pitch, f32 yaw, f32 z_near, f32 z_far)
{
	return (camera){position, aspect_ratio, horizontal_fov, pitch, yaw, z_near, z_far};
}

mat4 CameraTransform(camera camera)
{
	// TODO rotation
	return TranslationTransform(-camera.position.x, -camera.position.y, -camera.position.z);
}

mat4 CameraViewTransform(camera camera)
{
	f32 fov_factor = 1.f/tanf(camera.horizontal_fov/2.f);
	f32 z_scale = camera.z_far/(camera.z_far - camera.z_near);
	return (mat4){
		camera.aspect_ratio*fov_factor, 0.f, 0.f, 0.f,
		0.f, fov_factor, 0.f, 0.f,
		0.f, 0.f, z_scale, -z_scale*camera.z_near,
		0.f, 0.f, 1.f, 0.f
	};
}

mat4 ScaleTransform(f32 x, f32 y, f32 z)
{
	return (mat4){
		x, 0.f, 0.f, 0.f,
		0.f, y, 0.f, 0.f,
		0.f, 0.f, z, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

mat4 TranslationTransform(f32 x, f32 y, f32 z)
{
	return (mat4){
		0.f, 0.f, 0.f, -x,
		0.f, 0.f, 0.f, -y,
		0.f, 0.f, 0.f, -z,
		0.f, 0.f, 0.f, 1.f,
	};
}

mat4 RotationTransform(quat q)
{
	f32 wx = q.w*q.x;
	f32 wy = q.w*q.y;
	f32 wz = q.w*q.z;
	f32 xy = q.x*q.y;
	f32 xz = q.x*q.z;
	f32 yz = q.y*q.z;
	f32 xx = q.x*q.x;
	f32 yy = q.y*q.y;
	f32 zz = q.z*q.z;
	return (mat4){
		1.f - 2.f*(yy + zz), 2.f*(xy - wz), 2.f*(xz + wy), 0.f,
		2.f*(xy + wz), 1.f - 2.f*(xx + zz), 2.f*(yz - wx), 0.f,
		2.f*(xz - wy), 2.f*(yz + wx), 1.f - 2.f*(xx + yy), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
}

inline vec4 vec4PerspectiveDivide(vec4 v)
{
	v.x /= v.w;
	v.y /= v.w;
	v.z /= v.w;
	return v;
}
