#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ComponentStorage.h"
#include "ErrorHandling/ErrorMessage.h"

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
        auto it = m_storages.find(typeid(T));

        if (it == m_storages.end())
        {
            CreateError("Missing componentType!");
        }

        return *static_cast<ComponentStorage<T>*>(
            it->second.get()
            );
    }

private:
    std::unordered_map<
        std::type_index,
        std::shared_ptr<void>
    > m_storages;
};