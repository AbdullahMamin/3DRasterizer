#ifndef VECTOR_H
#include <assert.h>
#include <math.h>
#include "types.h"

typedef struct
{
      f32 x, y;
} vec2;

typedef struct
{
      f32 x, y, z;
} vec3;

typedef struct
{
      f32 x, y, z, w;
} vec4;

typedef struct
{
      f32 a11, a12,
          a21, a22;
} mat2;

typedef struct
{
      f32 a11, a12, a13,
          a21, a22, a23,
          a31, a32, a33;
} mat3;

typedef struct
{
      f32 a11, a12, a13, a14,
          a21, a22, a23, a24,
          a31, a32, a33, a34,
          a41, a42, a43, a44;
} mat4;

typedef struct
{
      f32 w, x, y, z;
} quat;

typedef struct
{
      f32 nx, ny, nz, d;
} plane;

vec2 Vec2(f32 x, f32 y);
vec2 vec2Add(vec2 v1, vec2 v2);
vec2 vec2Subtract(vec2 v1, vec2 v2);
vec2 vec2Multiply(vec2 v1, vec2 v2);
vec2 vec2Divide(vec2 v1, vec2 v2);
f32 vec2Dot(vec2 v1, vec2 v2);
f32 vec2Cross(vec2 v1, vec2 v2); // z-component
f32 vec2Magnitude(vec2 v);
vec2 vec2Normalize(vec2 v);
vec2 vec2Scale(f32 s, vec2 v);

vec3 Vec3(f32 x, f32 y, f32 z);
vec3 vec3Add(vec3 v1, vec3 v2);
vec3 vec3Subtract(vec3 v1, vec3 v2);
vec3 vec3Multiply(vec3 v1, vec3 v2);
vec3 vec3Divide(vec3 v1, vec3 v2);
f32 vec3Dot(vec3 v1, vec3 v2);
vec3 vec3Cross(vec3 v1, vec3 v2);
f32 vec3Magnitude(vec3 v);
vec3 vec3Normalize(vec3 v);
vec3 vec3Scale(f32 s, vec3 v);

vec4 Vec4(f32 x, f32 y, f32 z, f32 w);
vec4 Vec3ToVec4(vec3 v);
vec3 Vec4ToVec3(vec4 v);

mat2 mat2Add(mat2 m1, mat2 m2);
mat2 mat2Subtract(mat2 m1, mat2 m2);
mat2 mat2Multiply(mat2 m1, mat2 m2);
vec2 mat2MultiplyVec2(mat2 m, vec2 v);

mat3 mat3Add(mat3 m1, mat3 m2);
mat3 mat3Subtract(mat3 m1, mat3 m2);
mat3 mat3Multiply(mat3 m1, mat3 m2);
vec3 mat3MultiplyVec3(mat3 m, vec3 v);

mat4 mat4Add(mat4 m1, mat4 m2);
mat4 mat4Subtract(mat4 m1, mat4 m2);
mat4 mat4Multiply(mat4 m1, mat4 m2);
vec4 mat4MultiplyVec4(mat4 m, vec4 v);

quat Quat(vec3 axis, f32 angle);
quat quatMultiply(quat q1, quat q2);
quat quatConjugate(quat q);
f32 quatMagnitude(quat q);
quat quatNormalize(quat q);
vec3 vec3RotateByQuat(vec3 v, quat q);

bool isPointInPlane(vec3 point, plane plane);
f32 rayVsPlane(vec3 p1, vec3 p2, plane plane);

#endif // VECTOR_H
