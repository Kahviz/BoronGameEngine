#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <stdexcept>

#include "DirectXMath.h"
using namespace DirectX;

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 color;
    float brightness;
};

class Mesh
{
public:
    void Load(const std::string& file, ID3D11Device* device);
    void Draw(ID3D11DeviceContext* ctx) const;

    const std::vector<uint32_t>& GetIndices() const {
        return indices;
    }

    const std::vector<Vertex>& GetVertices() const {
        return verts;
    }
private:
    ID3D11Buffer* vb = nullptr;
    ID3D11Buffer* ib = nullptr;
    UINT indexCount = 0;
    std::vector<Vertex> verts;
    std::vector<uint32_t> indices;
};
