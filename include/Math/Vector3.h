#pragma once
#include <iostream>

struct Vector3
{
	float x;
	float y;
	float z;
};

inline std::ostream& operator<<(std::ostream& os, const Vector3& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

inline Vector3 operator+(const Vector3& a, const Vector3& b)
{
	return Vector3{ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vector3 operator-(const Vector3& a, const Vector3& b)
{
	return Vector3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vector3 operator*(const Vector3& a, float scalar)
{
	return Vector3{ a.x * scalar, a.y * scalar, a.z * scalar };
}

inline Vector3 operator/(const Vector3& a, float scalar)
{
	return Vector3{ a.x / scalar, a.y / scalar, a.z / scalar };
}

inline float dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3 Cross(const Vector3& a, const Vector3& b) {
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

inline float Length(const Vector3& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len == 0.0f) return Vector3(0, 0, 0);
	return v / len;
}
