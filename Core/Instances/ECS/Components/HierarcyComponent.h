#pragma once

#include <cstdint>

using EntityECS = uint32_t;

constexpr EntityECS INVALID_ENTITY = UINT32_MAX;

struct HierarchyComponent
{
    EntityECS parent = INVALID_ENTITY;
};