#include "ECS.h"
#include "ErrorHandling/ErrorMessage.h"

void ECS::init(ComponentManager* componentManager)
{
    m_componentManager = componentManager;

    m_componentManager->RegisterComponent<ColorComponent>();
    m_componentManager->RegisterComponent<EditorSettingsComponent>();
    m_componentManager->RegisterComponent<InstanceTypeComponent>();
    m_componentManager->RegisterComponent<ObjectComponent>();
    m_componentManager->RegisterComponent<TransformComponent>();
    m_componentManager->RegisterComponent<PhysicsComponent>();

    CreateSuccess("Inited ECS!");
}
uint32_t ECS::createEntity()
{
    return m_currentHandleID++;
}

void ECS::destroyEntity(EntityECS entity)
{
}

void ECS::Update(float deltaTime)
{
}

void ECS::Clear()
{
}
