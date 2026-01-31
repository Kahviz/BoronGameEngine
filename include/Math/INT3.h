#pragma once
#include <iostream>
//Part OF UntilitedMath


struct INT3
{
	int x;
	int y;
	int z;
};

inline std::ostream& operator<<(std::ostream& os, const INT3& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

inline INT3 operator+(const INT3& a, const INT3& b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline INT3 operator-(const INT3& a, const INT3& b)
{
	return INT3{ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline INT3 operator*(const INT3& a, int scalar)
{
	return INT3{ a.x * scalar, a.y * scalar, a.z * scalar };
}

inline INT3 operator/(const INT3& a, int scalar)
{
	return INT3{ a.x / scalar, a.y / scalar, a.z / scalar };
}

inline int dot(const INT3& a, const INT3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}