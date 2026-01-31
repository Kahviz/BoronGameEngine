#pragma once
#include <iostream>
#include "Vector4.h"

struct Matrix4x4
{
	Vector4 x;
	Vector4 y;
	Vector4 z;
	Vector4 w;
};
inline Matrix4x4 IdentityMatrix4x4() {
	return Matrix4x4{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	};
}

inline std::ostream& operator<<(std::ostream& os, const Matrix4x4& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

inline Matrix4x4 operator+(const Matrix4x4& a, const Matrix4x4& b)
{
	return Matrix4x4{
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w
	};
}

inline Matrix4x4 operator-(const Matrix4x4& a, const Matrix4x4& b)
{
	return Matrix4x4{
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w
	};
}

inline Matrix4x4 operator*(const Matrix4x4& a, float scalar)
{
	return Matrix4x4{
		a.x * scalar,
		a.y * scalar,
		a.z * scalar,
		a.w * scalar
	};
}

inline Matrix4x4 operator/(const Matrix4x4& a, float scalar)
{
	return Matrix4x4{ a.x / scalar, a.y / scalar, a.z / scalar , a.w / scalar };
}