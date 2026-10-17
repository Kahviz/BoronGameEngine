#pragma once
#include <cstdint>

#include "Frustrum/Frustrum.h"

inline uint32_t SHADOW_MAP_SIZE = 8076;
inline int SHADOW_MAP_UPDATE_INTERVAL = 2;
inline float ZFAR = 1000.0f;
inline float ZNEAR = 0.0f;

inline Frustum FRUSTRUM = {};