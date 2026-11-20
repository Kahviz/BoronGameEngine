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
    ID3D11Buffer* m_vb = nullptr;
    ID3D11Buffer* m_ib = nullptr;
    uint32_t m_indexCount = 0;
};
#endif