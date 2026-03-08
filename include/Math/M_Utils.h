#pragma once
#include "Variables.h"

inline float DegreesToRadians(float degrees)
{
	return degrees * (PI / 180.0f);
}

inline float RadiansToDegrees(float radians)
{
	return radians * (180.0f / PI);
}

template<typename T>
inline T Min(T a, T b) {
    return (a < b) ? a : b;
}

template<typename T>
inline T Max(T a, T b) {
	return (a < b) ? a : b;
}