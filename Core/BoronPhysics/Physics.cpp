#include "Physics.h"
#include "Components.h"

//Server
bool Physics::ApplyPhysics(ECS& p_ecs, EntityECS p_entity, float p_deltatime) {
    if (g_Running) {
        TransformComponent& transformComp = p_ecs.GetComponent<TransformComponent>(p_entity);
        PhysicsComponent& physicsComp = p_ecs.GetComponent<PhysicsComponent>(p_entity);

        BML::Vector3 pos = transformComp.transform.Position;
        BML::Vector3 size = transformComp.transform.Size;
        BML::Vector3 vel = physicsComp.Velocity;

        vel.y() -= GRAVITY * p_deltatime;
        pos.y() += vel.y() * p_deltatime;

        physicsComp.Velocity = vel;
        transformComp.transform.Position = pos;

        return true;
    }
    else {
        return false;
    }
}