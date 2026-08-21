#pragma once

#include <cstdint>

class Entity {
public:
    Entity() = default;
    explicit Entity(uint32_t id)
        : m_id(id)
    {
    }
    uint32_t getId(Entity entity) const {
        return entity.m_id;
    }

    uint32_t isValid() {
        return m_id != UINT32_MAX;
    }

    bool operator==(const Entity& other) const
    {
        return m_id == other.m_id;
    }

    bool operator!=(const Entity& other) const
    {
        return m_id != other.m_id;
    }
private:
    uint32_t m_id = UINT32_MAX;
};