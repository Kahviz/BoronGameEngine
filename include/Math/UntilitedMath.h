#pragma once

#include <cmath>
#include <limits>
#include "Math/INT3.h"
#include "Math/FLOAT3.h"
#include "Math/Vector4.h"
#include "Math/FLOAT4.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"

//Floats
constexpr float PI = 3.14159265f; //constexpr  aina sama arvo käännösaikana
constexpr float DOUBLE_PI = 2.0f * 3.14159265f;
constexpr float HALF_PI = 3.14159265f / 2.0f;
constexpr float Quarter_PI = 3.14159265f / 4.0f;
constexpr float CIRCLE_PI = PI * 2.0f;

constexpr float FLOAT_HUGE = 3.402823466e+38f;

constexpr float EARTH_GRAVITY = 9.80665f;
constexpr float MOON_GRAVITY = 1.625f;
//Ints
constexpr int INT_HUGE = 2147483647;
constexpr int INT_SMALL = -2147483648;
constexpr int INT_HALF_HUGE = INT_HUGE / 2;

//Utility functions
inline float DegreesToRadians(float degrees)
{
	return degrees * (PI / 180.0f);
}

inline float RadiansToDegrees(float radians)
{
	return radians * (180.0f / PI);
}

inline int MagnitudeINT3(INT3 a, INT3 b) {
	int dx = abs(a.x - b.x);
	int dz = abs(a.z - b.z);
	int dy = abs(a.y - b.y);

	return dx + dz + dy;
}
inline Vector3 Vector3TransformCoord(const Vector3& v, const Matrix4x4& m) {
	float x = v.x * m.x.x + v.y * m.y.x + v.z * m.z.x + m.w.x;
	float y = v.x * m.x.y + v.y * m.y.y + v.z * m.z.y + m.w.y;
	float z = v.x * m.x.z + v.y * m.y.z + v.z * m.z.z + m.w.z;
	float w = v.x * m.x.w + v.y * m.y.w + v.z * m.z.w + m.w.w;

	// Homogeeninen jako
	if (w != 0.0f) {
		x /= w;
		y /= w;
		z /= w;
	}

	return { x, y, z };
}


inline float MagnitudeFLOAT3(FLOAT3 a, FLOAT3 b) {
	float dx = abs(a.x - b.x);
	float dz = abs(a.z - b.z);
	float dy = abs(a.y - b.y);

	return dx + dz + dy;
}

inline Vector4 VectorSet(float x, float y, float z, float w) {
	return Vector4(x, y, z, w);
}

inline Vector3 VectorSet(float x, float y, float z) {
	return Vector3(x, y, z);
}


inline Vector4 LoadFloat3(FLOAT3 a) {
	return Vector4(
		a.x,
		a.y,
		a.z,
		0.0f
	);
}

inline Vector3 LoadFloat3Vec3(FLOAT3 a) {
	return Vector3(a.x, a.y, a.z);
}

inline Vector4 LoadInt3(INT3 a) {
	return Vector4(
		static_cast<float>(a.x),
		static_cast<float>(a.y),
		static_cast<float>(a.z),
		0.0f
	);
}

inline Matrix4x4 MatrixRotationRollPitchYaw(float pitch, float yaw, float roll) {
	float cp = cosf(pitch), sp = sinf(pitch);
	float cy = cosf(yaw), sy = sinf(yaw);
	float cr = cosf(roll), sr = sinf(roll);

	Matrix4x4 m;

	m.x = Vector4(cr * cy + sr * sp * sy, sr * cp, -cr * sy + sr * sp * cy, 0);
	m.y = Vector4(-sr * cy + cr * sp * sy, cr * cp, sr * sy + cr * sp * cy, 0);
	m.z = Vector4(cp * sy, -sp, cp * cy, 0);
	m.w = Vector4(0, 0, 0, 1);

	return m;
}

//XMVECTOR v = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
//XMFLOAT3 f3;
//XMStoreFloat3(&f3, v);
// f3.x = 1.0f, f3.y = 2.0f, f3.z = 3.0f

inline void StoreFloat3(FLOAT3* dest, const Vector4& src) {
	if (!dest) return;
	dest->x = src.x;
	dest->y = src.y;
	dest->z = src.z;
}


inline void StoreINT3(INT3& Int, Vector4& vec) {
	Int.x = static_cast<int>( vec.x );
	Int.y = static_cast<int>( vec.y );
	Int.z = static_cast<int>( vec.z );
}

inline Matrix4x4 MatrixLookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up) {
	Vector3 zaxis = { target.x - eye.x, target.y - eye.y, target.z - eye.z };
	float len = sqrtf(zaxis.x * zaxis.x + zaxis.y * zaxis.y + zaxis.z * zaxis.z);
	zaxis.x /= len; zaxis.y /= len; zaxis.z /= len;

	Vector3 xaxis = {
		up.y * zaxis.z - up.z * zaxis.y,
		up.z * zaxis.x - up.x * zaxis.z,
		up.x * zaxis.y - up.y * zaxis.x
	};
	len = sqrtf(xaxis.x * xaxis.x + xaxis.y * xaxis.y + xaxis.z * xaxis.z);
	xaxis.x /= len; xaxis.y /= len; xaxis.z /= len;

	Vector3 yaxis = {
		zaxis.y * xaxis.z - zaxis.z * xaxis.y,
		zaxis.z * xaxis.x - zaxis.x * xaxis.z,
		zaxis.x * xaxis.y - zaxis.y * xaxis.x
	};

	Matrix4x4 m;
	m.x = VectorSet(xaxis.x, yaxis.x, zaxis.x, 0);
	m.y = VectorSet(xaxis.y, yaxis.y, zaxis.y, 0);
	m.z = VectorSet(xaxis.z, yaxis.z, zaxis.z, 0);
	m.w = VectorSet(
		-(xaxis.x * eye.x + xaxis.y * eye.y + xaxis.z * eye.z),
		-(yaxis.x * eye.x + yaxis.y * eye.y + yaxis.z * eye.z),
		-(zaxis.x * eye.x + zaxis.y * eye.y + zaxis.z * eye.z),
		1
	);
	return m;
}


inline Matrix4x4 MatrixPerspectiveFovLH(float fovRadians, float aspect, float nearZ, float farZ) {
	float f = 1.0f / tanf(fovRadians / 2.0f);

	Matrix4x4 m = {};

	m.x = { f / aspect, 0, 0, 0 };
	m.y = { 0, f, 0, 0 };
	m.z = { 0, 0, farZ / (farZ - nearZ), 1 };
	m.w = { 0, 0, -nearZ * farZ / (farZ - nearZ), 0 };

	return m;
}

inline Vector4 Normalize(const Vector4& v) {
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (len == 0.0f) return Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	return Vector4(v.x / len, v.y / len, v.z / len, 0.0f);
}


inline Vector4 TransformNormal(const Vector4& v, const Matrix4x4& m)
{
	Vector4 result;
	result.x = v.x * m.x.x + v.y * m.y.x + v.z * m.z.x;
	result.y = v.x * m.x.y + v.y * m.y.y + v.z * m.z.y;
	result.z = v.x * m.x.z + v.y * m.y.z + v.z * m.z.z;
	result.w = 0.0f;
	return result;
}

inline Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m)
{
	Vector3 result;
	result.x = v.x * m.x.x + v.y * m.y.x + v.z * m.z.x;
	result.y = v.x * m.x.y + v.y * m.y.y + v.z * m.z.y;
	result.z = v.x * m.x.z + v.y * m.y.z + v.z * m.z.z;
	return result;
}

inline float Determinant3x3(float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3)
{
	return a1 * (b2 * c3 - b3 * c2) - a2 * (b1 * c3 - b3 * c1) + a3 * (b1 * c2 - b2 * c1);
}

inline Matrix4x4 InverseMatrix(const Matrix4x4& m) {
	Matrix4x4 inv;
	float det;

	// cofactors lasketaan jokaiselle elementille
	inv.x.x = Determinant3x3(m.y.y, m.y.z, m.y.w, m.z.y, m.z.z, m.z.w, m.w.y, m.w.z, m.w.w);
	inv.x.y = -Determinant3x3(m.y.x, m.y.z, m.y.w, m.z.x, m.z.z, m.z.w, m.w.x, m.w.z, m.w.w);
	inv.x.z = Determinant3x3(m.y.x, m.y.y, m.y.w, m.z.x, m.z.y, m.z.w, m.w.x, m.w.y, m.w.w);
	inv.x.w = -Determinant3x3(m.y.x, m.y.y, m.y.z, m.z.x, m.z.y, m.z.z, m.w.x, m.w.y, m.w.z);

	inv.y.x = -Determinant3x3(m.x.y, m.x.z, m.x.w, m.z.y, m.z.z, m.z.w, m.w.y, m.w.z, m.w.w);
	inv.y.y = Determinant3x3(m.x.x, m.x.z, m.x.w, m.z.x, m.z.z, m.z.w, m.w.x, m.w.z, m.w.w);
	inv.y.z = -Determinant3x3(m.x.x, m.x.y, m.x.w, m.z.x, m.z.y, m.z.w, m.w.x, m.w.y, m.w.w);
	inv.y.w = Determinant3x3(m.x.x, m.x.y, m.x.z, m.z.x, m.z.y, m.z.z, m.w.x, m.w.y, m.w.z);

	inv.z.x = Determinant3x3(m.x.y, m.x.z, m.x.w, m.y.y, m.y.z, m.y.w, m.w.y, m.w.z, m.w.w);
	inv.z.y = -Determinant3x3(m.x.x, m.x.z, m.x.w, m.y.x, m.y.z, m.y.w, m.w.x, m.w.z, m.w.w);
	inv.z.z = Determinant3x3(m.x.x, m.x.y, m.x.w, m.y.x, m.y.y, m.y.w, m.w.x, m.w.y, m.w.w);
	inv.z.w = -Determinant3x3(m.x.x, m.x.y, m.x.z, m.y.x, m.y.y, m.y.z, m.w.x, m.w.y, m.w.z);

	inv.w.x = -Determinant3x3(m.x.y, m.x.z, m.x.w, m.y.y, m.y.z, m.y.w, m.z.y, m.z.z, m.z.w);
	inv.w.y = Determinant3x3(m.x.x, m.x.z, m.x.w, m.y.x, m.y.z, m.y.w, m.z.x, m.z.z, m.z.w);
	inv.w.z = -Determinant3x3(m.x.x, m.x.y, m.x.w, m.y.x, m.y.y, m.y.w, m.z.x, m.z.y, m.z.w);
	inv.w.w = Determinant3x3(m.x.x, m.x.y, m.x.z, m.y.x, m.y.y, m.y.z, m.z.x, m.z.y, m.z.z);

	// determinant
	det = m.x.x * inv.x.x + m.x.y * inv.y.x + m.x.z * inv.z.x + m.x.w * inv.w.x;
	if (fabs(det) < 1e-6f) throw std::runtime_error("Matrix not invertible!");

	// jaetaan kaikki determinantilla
	float invDet = 1.0f / det;
	inv.x.x *= invDet; inv.x.y *= invDet; inv.x.z *= invDet; inv.x.w *= invDet;
	inv.y.x *= invDet; inv.y.y *= invDet; inv.y.z *= invDet; inv.y.w *= invDet;
	inv.z.x *= invDet; inv.z.y *= invDet; inv.z.z *= invDet; inv.z.w *= invDet;
	inv.w.x *= invDet; inv.w.y *= invDet; inv.w.z *= invDet; inv.w.w *= invDet;

	return inv;
}


//float x = 9.0f;
//float y = std::sqrt(x); // y = 3.0f