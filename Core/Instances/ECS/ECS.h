#pragma once

#include <cstdint>
#include "Entity.h"
#include "ComponentManager.h"
#include <tuple>

using EntityECS = uint32_t;

class ECS {
public:
	void init(ComponentManager* componentManager);

	uint32_t createEntity();
	void destroyEntity(EntityECS entity);

	template<typename T>
	void AddComponent(EntityECS entity, const T& component)
	{
		m_componentManager->GetStorage<T>().Add(entity, component);
	}

	template<typename T>
	void RemoveComponent(EntityECS entity)
	{
	}

	template<typename T>
	bool HasComponent(EntityECS entity)
	{
		return m_componentManager->GetStorage<T>().Has(entity);
	}

	template<typename T>
	inline T& GetComponent(EntityECS entity)
	{
		return m_componentManager->GetStorage<T>().Get(entity);
	}

	template<typename Func>
	void EachEntity(Func&& function)
	{
		for (EntityECS entity = 0;
			entity < m_currentHandleID + 1;
			++entity)
		{
			function(entity);
		}
	}

	template<typename... Components, typename Func>
	void Each(Func&& function)
	{
		static_assert(
			sizeof...(Components) > 0,
			"ECS::Each requires at least one component"
			);

		using FirstComponent =
			std::tuple_element_t<0, std::tuple<Components...>>;

		auto& storage =
			m_componentManager->GetStorage<FirstComponent>();

		storage.ForEach(
			[&](EntityECS entity, FirstComponent&)
			{
				if ((HasComponent<Components>(entity) && ...))
				{
					function(
						entity,
						GetComponent<Components>(entity)...
					);
				}
			}
		);
	}

	void DeselectAll();
	void Update(float deltaTime);
	void Clear();

	uint32_t getNumberOfEntities() { return m_currentHandleID; }
private:
	ComponentManager* m_componentManager = nullptr;
	uint32_t m_currentHandleID = 1;
};