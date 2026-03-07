#pragma once
#include <iostream>

struct FLOAT2
{
	float x;
	float y;
	float z;

	inline FLOAT2& operator+=(const FLOAT2& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	inline FLOAT2& operator-=(const FLOAT2& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
};

inline std::ostream& operator<<(std::ostream& os, const FLOAT2& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

inline FLOAT2 operator+(const FLOAT2& a, const FLOAT2& b)
{
	return FLOAT2{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline FLOAT2 operator-(const FLOAT2& a, const FLOAT2& b)
{
	return FLOAT2{ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline FLOAT2 operator*(const FLOAT2& a, float scalar)
{
	return FLOAT2{ a.x * scalar, a.y * scalar, a.z * scalar };
}

inline FLOAT2 operator/(const FLOAT2& a, float scalar)
{
	return FLOAT2{ a.x / scalar, a.y / scalar, a.z / scalar };
}

inline float dot(const FLOAT2& a, const FLOAT2& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float MagnitudeFLOAT2(FLOAT2 a, FLOAT2 b) {
	float dx = abs(a.x - b.x);
	float dz = abs(a.z - b.z);
	float dy = abs(a.y - b.y);

	return dx + dz + dy;
}
