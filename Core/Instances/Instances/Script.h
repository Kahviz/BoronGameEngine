#pragma once

#include <vector>
#include <string>

#include "Instances/Instance.h"
#include "BoronMathLibrary.h"
#include "Instances/Vertex.h"
#include "Instances/Instances/Mesh/Mesh.h"

class Script : public Instance {
public:
    std::string CodeTag;
    std::string Code;

    Script(
        const std::string& name,
        int instanceID,
        const BML::Int3& col,
        const BML::Int3& ogcol,
        const BML::Vector3& velocity,
        const Transform& transform,
        bool anchored,
        std::shared_ptr<Mesh> mesh
    )
        : Instance(
            name,
            col,
            ogcol,
            velocity,
            mesh,
            instanceID,
            false,
            false,
            true,
            transform,
            anchored,
            Boron::Enums::InstanceType::Script
        ),
        CodeTag("")
    {
    }

    bool CanDraw() const override { return false; }
    bool HaveColor() const override { return false; }
    bool HaveSize() const override { return false; }
    bool HavePos() const override { return false; }
    bool HaveOrientation() const override { return false; }
    bool ShowsInExplorer() const override { return true; }
    bool HaveVelocity() const override { return false; }
    bool HaveAnchored() const override { return false; }
    bool HaveOBJMesh() const override { return false; }
    bool HasTexture() const override { return false; }

    void Deselect() override {
        Selected = false;
        color = originalColor;
    }
};