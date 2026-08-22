#include "ECS.h"
#include "ErrorHandling/ErrorMessage.h"
#include "Components.h"

void ECS::init(ComponentManager* componentManager)
{
    m_componentManager = componentManager;

    m_componentManager->RegisterComponent<BasicInfoComponent>();
    m_componentManager->RegisterComponent<ColorComponent>();
    m_componentManager->RegisterComponent<TransformComponent>();
    m_componentManager->RegisterComponent<PhysicsComponent>();
    m_componentManager->RegisterComponent<ObjectComponent>();
    m_componentManager->RegisterComponent<EditorSettingsComponent>();
    m_componentManager->RegisterComponent<HierarcyComponent>();
    m_componentManager->RegisterComponent<TextureComponent>();
    m_componentManager->RegisterComponent<InstanceTypeComponent>();

    CreateSuccess("Inited ECS!");
}
uint32_t ECS::createEntity()
{
    return m_currentHandleID++;
}

void ECS::destroyEntity(EntityECS entity)
{
}

void ECS::DeselectAll()
{
    Each<EditorSettingsComponent>(
        [](EntityECS entity, EditorSettingsComponent& editor)
        {
            editor.selected = false;
        }
    );
}

void ECS::Update(float deltaTime)
{
}

void ECS::Clear()
{
}
