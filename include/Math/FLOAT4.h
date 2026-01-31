#pragma once
#include <iostream>

//Part OF UntilitedMath
struct FLOAT4
{
	float x;
	float y;
	float z;
	float w;
};

inline std::ostream& operator<<(std::ostream& os, const FLOAT4& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

inline FLOAT4 operator+(const FLOAT4& a, const FLOAT4& b)
{
	return FLOAT4{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}

inline FLOAT4 operator-(const FLOAT4& a, const FLOAT4& b)
{
	return FLOAT4{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}

inline FLOAT4 operator*(const FLOAT4& a, float scalar)
{
	return FLOAT4{ a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar };
}

inline FLOAT4 operator/(const FLOAT4& a, float scalar)
{
	return FLOAT4{ a.x / scalar, a.y / scalar, a.z / scalar , a.w / scalar };
}

inline float dot(const FLOAT4& a, const FLOAT4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}