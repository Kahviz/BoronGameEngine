#pragma once

#include <cstdint>
#include <string>
#include "Logger/Logger.h"

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

    bool isValid() {
        bool isValid = m_id != UINT32_MAX;

        if (!isValid) {
            CreateError("Entity: " + std::to_string(this->getId(*this)) + "is not valid");
        }

        return isValid;
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