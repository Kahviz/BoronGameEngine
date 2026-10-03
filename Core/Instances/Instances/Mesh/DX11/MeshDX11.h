#pragma once

#include "GLOBALS.h"

#if DIRECTX11 == 1
#include "DX11.h"
#include <vector>
#include "Instances/Vertex.h"
#include "Mesh/Mesh.h"

class MeshBackend : public Mesh {
public:
    void LoadMesh(const fs::path& p_file, MeshStruct& p_meshStruct);
    void Draw(MeshDrawStruct& p_meshDrawStruct) const override;
private:
    ID3D11Buffer* vb = nullptr;
    ID3D11Buffer* ib = nullptr;
    uint32_t indexCount = 0;
};
#endif