#pragma once

#include <vector>
#include <string>

#include "Instances/Instance.h"
#include "BoronMathLibrary.h"
#include "Instances/Vertex.h"
#include "Instances/Instances/Mesh/Mesh.h"
#include "Graphics/Texture/Texture.h"
#include "Services/Raycasting.h"

inline BML::Vector3 LoadWorldVertex(const Vertex& v, const BML::Vector3& objPos, const BML::Vector3& objSize) {
    return BML::Vector3(
        v.pos.x * objSize.x() + objPos.x(),
        v.pos.y * objSize.y() + objPos.y(),
        v.pos.z * objSize.z() + objPos.z()
    );
}

class Object : public Instance {
public:
    std::string Path;
    std::string CodeTag;
    Texture texture;

    Object(
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
            0,
            instanceID,
            false,
            false,
            true,
            transform,
            anchored,
            Boron::Enums::InstanceType::Object
        ),
        Path(""),
        CodeTag(""),
        texture()
    {
    }

    Texture* GetTexture() override { return &texture; }
    const Texture* GetConstTexture() const { return &texture; }

    bool CanDraw() const override { return true; }
    bool HaveColor() const override { return true; }
    bool HaveSize() const override { return true; }
    bool HavePos() const override { return true; }
    bool HaveOrientation() const override { return true; }
    bool ShowsInExplorer() const override { return true; }
    bool HaveVelocity() const override { return true; }
    bool HaveAnchored() const override { return true; }
    bool HaveOBJMesh() const override { return true; }
    bool HasTexture() const override { return true; }

    bool RayIntersects(const BML::Vector3& rayOrigin, const BML::Vector3& rayDir) override {
        if (!OBJmesh || OBJmesh->GetVertices().size() < 3)
            return false;

        const auto& vertices = OBJmesh->GetVertices();
        const auto& indices = OBJmesh->GetIndices();

        const BML::Vector3& pos = transform.Position;
        const BML::Vector3& size = transform.Size;

        static Raycasting ray;

        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            BML::Vector3 v0 = LoadWorldVertex(vertices[indices[i]], pos, size);
            BML::Vector3 v1 = LoadWorldVertex(vertices[indices[i + 1]], pos, size);
            BML::Vector3 v2 = LoadWorldVertex(vertices[indices[i + 2]], pos, size);

            float t;
            if (ray.RayIntersectsTriangle(rayOrigin, rayDir, v0, v1, v2, t))
                return true;
        }

        return false;
    }

    void Deselect() override {
        Selected = false;
        color = originalColor;
    }
};