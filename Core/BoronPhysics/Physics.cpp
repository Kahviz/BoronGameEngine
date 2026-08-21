#include "Physics.h"

//Server
bool Physics::ApplyGravity(ECS& ecs,EntityECS entity, float deltatime)
{
    if (g_Running) {
        TransformComponent& transformComp = ecs.GetComponent<TransformComponent>(entity);
        PhysicsComponent& physicsComp = ecs.GetComponent<PhysicsComponent>(entity);

        BML::Vector3 pos = transformComp.transform.Position;
        BML::Vector3 vel = physicsComp.Velocity;

        vel.y() -= GRAVITY * deltatime;
        pos.y() += vel.y() * deltatime;

        physicsComp.Velocity = vel;
        transformComp.transform.Position = pos;

        return true;
    }
    else {
        return false;
    }
}