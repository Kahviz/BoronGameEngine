#pragma once

#include <cstdint>
#include "Entity.h"
#include "ComponentManager.h"
#include "Components.h"

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
	bool HasComponent(uint32_t entity, const T& component)
	{
		return false;
	}

	template<typename T>
	inline T& GetComponent(EntityECS entity)
	{
		return m_componentManager->GetStorage<T>().Get(entity);
	}

	template<typename ...Components, typename Func>
	void Each(Func&& function)
	{
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


	void Update(float deltaTime);
	void Clear();
private:
	ComponentManager* m_componentManager = nullptr;
	uint32_t m_currentHandleID = 0;
};