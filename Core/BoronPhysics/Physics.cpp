#include "Physics.h"

//Server
bool Physics::ApplyGravity(Instance& inst, float Deltatime)
{
    if (Running) {
        Vector3 pos = inst.transform.Position;
        Vector3 vel = inst.Velocity;

        vel.y() -= GRAVITY * Deltatime;
        pos.y() += vel.y() * Deltatime;

        inst.Velocity = vel;
        inst.transform.Position = pos;

        return true;
    }
    else {
        return false;
    }
}