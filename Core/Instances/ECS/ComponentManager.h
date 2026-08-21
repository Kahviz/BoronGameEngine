#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ComponentStorage.h"

class ComponentManager
{
public:

    template<typename T>
    void RegisterComponent()
    {
        m_storages[typeid(T)] =
            std::make_shared<ComponentStorage<T>>();
    }

    template<typename T>
    ComponentStorage<T>& GetStorage()
    {
        return *std::static_pointer_cast<ComponentStorage<T>>(
            m_storages.at(typeid(T))
        );
    }

private:
    std::unordered_map<
        std::type_index,
        std::shared_ptr<void>
    > m_storages;
};