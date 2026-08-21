#pragma once
#include <Mesh/Mesh.h>

struct ObjectComponent {
    std::shared_ptr<Mesh> OBJmesh;
    bool canDraw = true;
};