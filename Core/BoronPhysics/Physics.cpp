#include "Physics.h"

//Server
bool Physics::ApplyGravity(Instance& inst, float deltatime)
{
    if (Running) {
        BML::Vector3 pos = inst.transform.Position;
        BML::Vector3 vel = inst.Velocity;

        vel.y() -= GRAVITY * deltatime;
        pos.y() += vel.y() * deltatime;

        inst.Velocity = vel;
        inst.transform.Position = pos;

        return true;
    }
    else {
        return false;
    }
}