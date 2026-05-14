#pragma once

#include <vector>
#include <string>
#include <memory>

#include "BoronMathLibrary.h"
#include "Instances/Instances/Mesh/Mesh.h"
#include "Enums.h"

class Texture;

class Instance {
public:
    std::shared_ptr<Mesh> OBJmesh;
    std::string Name;

    Instance* Parent = nullptr;
    std::vector<Instance*> Children;

    Vector3 Velocity;
    Transform transform;

    Boron::Enums::InstanceType InstanceType;

    Int3 color;
    Int3 originalColor;

    int UniqueID = 0;
    int InstanceID = 0;

    bool Anchored = true;
    bool Selected = false;
    bool Deleted = false;
    bool IsVisibleInExplorer = false;

    Instance(
        const std::string& name = "Instance",
        const Int3& COLOR = { 0, 0, 0 },
        const Int3& OGCOLOR = { 0, 0, 0 },
        const Vector3& VELOCITY = { 0, 0, 0 },
        const std::shared_ptr<Mesh>& MESH = std::make_shared<Mesh>(),
        int uniqueID = 0,
        int instanceID = 0,
        bool selected = false,
        bool deleted = false,
        bool isvisibleinexplorer = false,
        const Transform& TRANSFORM = {},
        bool ANCHORED = false,
        Boron::Enums::InstanceType instancetype = Boron::Enums::InstanceType::Object
    )
        : OBJmesh(MESH),
        Velocity(VELOCITY),
        transform(TRANSFORM),
        Anchored(ANCHORED),
        Name(name),
        color(COLOR),
        originalColor(OGCOLOR),
        UniqueID(uniqueID),
        InstanceID(instanceID),
        Selected(selected),
        Deleted(deleted),
        IsVisibleInExplorer(isvisibleinexplorer),
        InstanceType(instancetype)
    {
    }

    virtual ~Instance() = default;

    virtual Texture* GetTexture() { return nullptr; }
    virtual const Texture* GetConstTexture() const { return nullptr; }

    virtual bool CanDraw() const { return false; }
    virtual bool HasTexture() const { return false; }
    virtual bool HaveColor() const { return false; }
    virtual bool HaveSize() const { return false; }
    virtual bool HavePos() const { return false; }
    virtual bool HaveOrientation() const { return false; }
    virtual bool ShowsInExplorer() const { return false; }

    virtual bool HaveVelocity() const { return false; }
    virtual bool HaveAnchored() const { return false; }
    virtual bool HaveOBJMesh() const { return false; }

    virtual bool RayIntersects(const Vector3& rayOrigin, const Vector3& rayDir) { return false; }

    virtual const std::vector<Instance*>& GetChildren() const {
        return Children;
    }

    virtual void Select() { Selected = true; }
    virtual void Deselect() { Selected = false; }

    virtual bool IsChild() const {
        return Parent && Parent->InstanceType != Boron::Enums::InstanceType::World;
    }
};