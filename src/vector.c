#include "vector.h"

inline vec2 Vec2(f32 x, f32 y)
{
	return (vec2){x, y};
}

inline vec2 vec2Add(vec2 v1, vec2 v2)
{
	return (vec2){v1.x + v2.x, v1.y + v2.y};
}

inline vec2 vec2Subtract(vec2 v1, vec2 v2)
{
	return (vec2){v1.x - v2.x, v1.y - v2.y};
}

inline vec2 vec2Multiply(vec2 v1, vec2 v2)
{
	return (vec2){v1.x*v2.x, v1.y*v2.y};
}

inline vec2 vec2Divide(vec2 v1, vec2 v2)
{
	return (vec2){v1.x/v2.x, v1.y/v2.y};
}

inline f32 vec2Dot(vec2 v1, vec2 v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

inline f32 vec2Cross(vec2 v1, vec2 v2)
{
	return v1.x*v2.y - v1.y*v2.x;
}

inline f32 vec2Magnitude(vec2 v)
{
	return sqrtf(v.x*v.x + v.y*v.y);
}

inline vec2 vec2Normalize(vec2 v)
{
	f32 inv_magnitude = 1.f/sqrtf(v.x*v.x + v.y*v.y);
	return (vec2){inv_magnitude*v.x, inv_magnitude*v.y};
}

inline vec2 vec2Scale(f32 s, vec2 v)
{
	return (vec2){s*v.x, s*v.y};
}

inline vec3 Vec3(f32 x, f32 y, f32 z)
{
	return (vec3){x, y, z};
}

inline vec3 vec3Add(vec3 v1, vec3 v2)
{
	return (vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

inline vec3 vec3Subtract(vec3 v1, vec3 v2)
{
	return (vec3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

inline vec3 vec3Multiply(vec3 v1, vec3 v2)
{
	return (vec3){v1.x*v2.x, v1.y*v2.y, v1.z*v2.z};
}

inline vec3 vec3Divide(vec3 v1, vec3 v2)
{
	return (vec3){v1.x/v2.x, v1.y/v2.y, v1.z/v2.z};
}

inline f32 vec3Dot(vec3 v1, vec3 v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

inline vec3 vec3Cross(vec3 v1, vec3 v2)
{
	return (vec3){v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}

inline f32 vec3Magnitude(vec3 v)
{
	return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline vec3 vec3Normalize(vec3 v)
{
	f32 inv_magnitude = 1.f/sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
	return (vec3){inv_magnitude*v.x, inv_magnitude*v.y, inv_magnitude*v.z};
}

inline vec3 vec3Scale(f32 s, vec3 v)
{
	return (vec3){s*v.x, s*v.y, s*v.z};
}

inline vec4 Vec4(f32 x, f32 y, f32 z, f32 w)
{
	return (vec4){x, y, z, w};
}

inline vec4 Vec3ToVec4(vec3 v)
{
	return (vec4){v.x, v.y, v.z, 1.f};
}

inline vec3 Vec4ToVec3(vec4 v)
{
	return (vec3){v.x, v.y, v.z};
}

inline mat2 mat2Add(mat2 m1, mat2 m2)
{
	return (mat2){
		m1.a11 + m2.a11, m1.a12 + m2.a12,
		m1.a21 + m2.a21, m1.a22 + m2.a22
	};
}

inline mat2 mat2Subtract(mat2 m1, mat2 m2)
{
	return (mat2){
		m1.a11 - m2.a11, m1.a12 - m2.a12,
		m1.a21 - m2.a21, m1.a22 - m2.a22
	};
}

inline mat2 mat2Multiply(mat2 m1, mat2 m2)
{
	return (mat2){
		m1.a11*m2.a11 + m1.a12*m2.a21, m1.a11*m2.a12 + m1.a12*m2.a22,
		m1.a21*m2.a11 + m1.a22*m2.a21, m1.a21*m2.a12 + m1.a22*m2.a22
	};
}

inline vec2 mat2MultiplyVec2(mat2 m, vec2 v)
{
	return (vec2){
		m.a11*v.x + m.a12*v.y,
		m.a21*v.x + m.a22*v.y
	};
}

inline mat3 mat3Add(mat3 m1, mat3 m2)
{
	return (mat3){
		m1.a11 + m2.a11, m1.a12 + m2.a12, m1.a13 + m2.a13,
		m1.a21 + m2.a21, m1.a22 + m2.a22, m1.a23 + m2.a23,
		m1.a31 + m2.a31, m1.a32 + m2.a32, m1.a33 + m2.a33
	};
}

mat3 mat3Subtract(mat3 m1, mat3 m2)
{
	return (mat3){
		m1.a11 - m2.a11, m1.a12 - m2.a12, m1.a13 - m2.a13,
		m1.a21 - m2.a21, m1.a22 - m2.a22, m1.a23 - m2.a23,
		m1.a31 - m2.a31, m1.a32 - m2.a32, m1.a33 - m2.a33
	};
}

inline mat3 mat3Multiply(mat3 m1, mat3 m2)
{
	return (mat3){
		m1.a11*m2.a11 + m1.a12*m2.a21 + m1.a13*m2.a31, m1.a11*m2.a12 + m1.a12*m2.a22 + m1.a13*m2.a32, m1.a11*m2.a13 + m1.a12*m2.a23 + m1.a13*m2.a33,
		m1.a21*m2.a11 + m1.a22*m2.a21 + m1.a23*m2.a31, m1.a21*m2.a12 + m1.a22*m2.a22 + m1.a23*m2.a32, m1.a21*m2.a13 + m1.a22*m2.a23 + m1.a23*m2.a33,
		m1.a31*m2.a11 + m1.a32*m2.a21 + m1.a33*m2.a31, m1.a31*m2.a12 + m1.a32*m2.a22 + m1.a33*m2.a32, m1.a31*m2.a13 + m1.a32*m2.a23 + m1.a33*m2.a33
	};
}

inline vec3 mat3MultiplyVec3(mat3 m, vec3 v)
{
	return (vec3){
		m.a11*v.x + m.a12*v.y + m.a13*v.z,
		m.a21*v.x + m.a22*v.y + m.a23*v.z,
		m.a31*v.x + m.a32*v.y + m.a33*v.z
	};
}

inline mat4 mat4Add(mat4 m1, mat4 m2)
{
	return (mat4){
		m1.a11 + m2.a11, m1.a12 + m2.a12, m1.a13 + m2.a13, m1.a14 + m2.a14,
		m1.a21 + m2.a21, m1.a22 + m2.a22, m1.a23 + m2.a23, m1.a24 + m2.a24,
		m1.a31 + m2.a31, m1.a32 + m2.a32, m1.a33 + m2.a33, m1.a34 + m2.a34,
		m1.a41 + m2.a41, m1.a42 + m2.a42, m1.a43 + m2.a43, m1.a44 + m2.a44
	};
}

mat4 mat4Subtract(mat4 m1, mat4 m2)
{
	return (mat4){
		m1.a11 - m2.a11, m1.a12 - m2.a12, m1.a13 - m2.a13, m1.a14 - m2.a14,
		m1.a21 - m2.a21, m1.a22 - m2.a22, m1.a23 - m2.a23, m1.a24 - m2.a24,
		m1.a31 - m2.a31, m1.a32 - m2.a32, m1.a33 - m2.a33, m1.a34 - m2.a34,
		m1.a41 - m2.a41, m1.a42 - m2.a42, m1.a43 - m2.a43, m1.a44 - m2.a44
	};
}

inline mat4 mat4Multiply(mat4 m1, mat4 m2)
{
	return (mat4){
		m1.a11*m2.a11 + m1.a12*m2.a21 + m1.a13*m2.a31 + m1.a14*m2.a41, m1.a11*m2.a12 + m1.a12*m2.a22 + m1.a13*m2.a32 + m1.a14*m2.a42, m1.a11*m2.a13 + m1.a12*m2.a23 + m1.a13*m2.a33 + m1.a14*m2.a43, m1.a11*m2.a14 + m1.a12*m2.a24 + m1.a13*m2.a34 + m1.a14*m2.a44,
		m1.a21*m2.a11 + m1.a22*m2.a21 + m1.a23*m2.a31 + m1.a24*m2.a41, m1.a21*m2.a12 + m1.a22*m2.a22 + m1.a23*m2.a32 + m1.a24*m2.a42, m1.a21*m2.a13 + m1.a22*m2.a23 + m1.a23*m2.a33 + m1.a24*m2.a43, m1.a21*m2.a14 + m1.a22*m2.a24 + m1.a23*m2.a34 + m1.a24*m2.a44,
		m1.a31*m2.a11 + m1.a32*m2.a21 + m1.a33*m2.a31 + m1.a34*m2.a41, m1.a31*m2.a12 + m1.a32*m2.a22 + m1.a33*m2.a32 + m1.a34*m2.a42, m1.a31*m2.a13 + m1.a32*m2.a23 + m1.a33*m2.a33 + m1.a34*m2.a43, m1.a31*m2.a14 + m1.a32*m2.a24 + m1.a33*m2.a34 + m1.a34*m2.a44,
		m1.a41*m2.a11 + m1.a42*m2.a21 + m1.a43*m2.a31 + m1.a44*m2.a41, m1.a41*m2.a12 + m1.a42*m2.a22 + m1.a43*m2.a32 + m1.a44*m2.a42, m1.a41*m2.a13 + m1.a42*m2.a23 + m1.a43*m2.a33 + m1.a44*m2.a43, m1.a41*m2.a14 + m1.a42*m2.a24 + m1.a43*m2.a34 + m1.a44*m2.a44
	};
}

inline vec4 mat4MultiplyVec4(mat4 m, vec4 v)
{
	return (vec4){
		m.a11*v.x + m.a12*v.y + m.a13*v.z + m.a14*v.w,
		m.a21*v.x + m.a22*v.y + m.a23*v.z + m.a24*v.w,
		m.a31*v.x + m.a32*v.y + m.a33*v.z + m.a34*v.w,
		m.a41*v.x + m.a42*v.y + m.a43*v.z + m.a44*v.w
	};
}

quat Quat(vec3 axis, f32 angle)
{
	assert(fabs(vec3Magnitude(axis) - 1.f) < 0.0001f); // Make sure axis is a unit vector
	f32 cosine = cosf(angle/2.f);
	f32 sine = sinf(angle/2.f);
	return (quat){cosine, axis.x*sine, axis.y*sine, axis.z*sine};
}

quat quatMultiply(quat q1, quat q2)
{
	return (quat){
		q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z,
		q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y,
		q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x,
		q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w
	};
}

quat quatConjugate(quat q)
{
	return (quat){q.w, -q.x, -q.y, -q.z};
}

f32 quatMagnitude(quat q)
{
	return sqrtf(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
}

quat quatNormalize(quat q)
{
	f32 inv_magnitude = 1.f/sqrtf(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
	return (quat){inv_magnitude*q.w, inv_magnitude*q.x, inv_magnitude*q.y, inv_magnitude*q.z};
}

vec3 vec3RotateByQuat(vec3 v, quat q)
{
	quat v_as_quat = (quat){0.f, v.x, v.y, v.z};
	v_as_quat = quatMultiply(quatMultiply(q, v_as_quat), quatConjugate(q));
	return (vec3){v_as_quat.x, v_as_quat.y, v_as_quat.z};
}

bool isPointInPlane(vec3 point, plane plane)
{
	vec3 normal = Vec3(plane.nx, plane.ny, plane.nz);
	vec3 plane_point = vec3Scale(plane.d, normal);
	return vec3Dot(normal,  vec3Subtract(point, plane_point)) > 0.f;
}

f32 rayVsPlane(vec3 p1, vec3 p2, plane plane)
{
	vec3 normal = Vec3(plane.nx, plane.ny, plane.nz);
	return -vec3Dot(p1, normal)/vec3Dot(vec3Subtract(p2, p1), normal);
}

void printMat4(mat4 m)
{
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
		m.a11, m.a12, m.a13, m.a14,
		m.a21, m.a22, m.a23, m.a24,
		m.a31, m.a32, m.a33, m.a34,
		m.a41, m.a42, m.a43, m.a44
	);
	puts("----");
}
