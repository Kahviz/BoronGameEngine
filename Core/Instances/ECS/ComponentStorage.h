#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>

template<typename T> 
class ComponentStorage {
public:
	void Add(uint32_t entity, const T& component) {
		entityToIndex[entity] = components.size();
        components.push_back(component);
	}

    T& Get(uint32_t entity) {
        return components[entityToIndex.at(entity)];
	}

	bool Has(uint32_t entity) const {
		return entityToIndex.contains(entity);
	}

    void Remove(Entity entity)
    {
        auto it = entityToIndex.find(entity);

        if (it == entityToIndex.end())
            return;

        size_t index = it->second;
        size_t last = components.size() - 1;

        if (index != last)
        {
            components[index] = std::move(components[last]);
        }

        components.pop_back();
        entityToIndex.erase(it);
    }
private:
	std::vector<T> components;
	std::unordered_map<uint32_t, size_t> entityToIndex;
};