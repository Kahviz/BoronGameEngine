#pragma once
#include <iostream>

//Part OF UntilitedMath
struct FLOAT3
{
	float x;
	float y;
	float z;

	inline FLOAT3& operator+=(const FLOAT3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	inline FLOAT3& operator-=(const FLOAT3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
};

inline std::ostream& operator<<(std::ostream& os, const FLOAT3& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

inline FLOAT3 operator+(const FLOAT3& a, const FLOAT3& b)
{
	return FLOAT3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline FLOAT3 operator-(const FLOAT3& a, const FLOAT3& b)
{
	return FLOAT3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline FLOAT3 operator*(const FLOAT3& a, float scalar)
{
	return FLOAT3{ a.x * scalar, a.y * scalar, a.z * scalar };
}

inline FLOAT3 operator/(const FLOAT3& a, float scalar)
{
	return FLOAT3{ a.x / scalar, a.y / scalar, a.z / scalar };
}

inline float dot(const FLOAT3& a, const FLOAT3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}