#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <utility>

using EntityECS = uint32_t;

template<typename T>
class ComponentStorage {
public:

    void Add(EntityECS entity, const T& component)
    {
        auto it = entityToIndex.find(entity);

        if (it != entityToIndex.end())
        {
            components[it->second] = component;
            return;
        }

        entityToIndex[entity] = components.size();
        components.push_back(component);
        entities.push_back(entity);
    }

    T& Get(EntityECS entity)
    {
        return components[entityToIndex.at(entity)];
    }

    bool Has(EntityECS entity) const
    {
        return entityToIndex.contains(entity);
    }

    void Remove(EntityECS entity)
    {
        auto it = entityToIndex.find(entity);

        if (it == entityToIndex.end())
            return;

        size_t index = it->second;
        size_t last = components.size() - 1;

        if (index != last)
        {
            components[index] = std::move(components[last]);

            EntityECS movedEntity = entities[last];

            entities[index] = movedEntity;
            entityToIndex[movedEntity] = index;
        }

        components.pop_back();
        entities.pop_back();
        entityToIndex.erase(it);
    }

    template<typename Func>
    void ForEach(Func&& function)
    {
        for (size_t i = 0; i < components.size(); ++i)
        {
            function(entities[i], components[i]);
        }
    }

private:
    std::vector<T> components;
    std::vector<EntityECS> entities;
    std::unordered_map<EntityECS, size_t> entityToIndex;
};