#include "MeshDX11.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#if DIRECTX11 == 1
void MeshBackend::LoadMesh(const fs::path& p_file, MeshStruct& p_meshStruct) {
    Assimp::Importer imp{};
    const aiScene* scene = imp.ReadFile(
        p_file.string(),
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals
    );

    if (!scene || !scene->HasMeshes())
        throw std::runtime_error("Failed to load model: " + std::string(imp.GetErrorString()));

    aiMesh* m = scene->mMeshes[0];
    if (!m || m->mNumVertices == 0) {
        CreateError("Invalid mesh data");
    }

    if (!m->HasNormals()) {
        CreateError("Mesh has no normals!");
    }

    //Vertices
    getVerticesMod().resize(m->mNumVertices);

    for (UINT i = 0; i < m->mNumVertices; ++i)
    {
        getVerticesMod()[i].brightness = 1.0f;

        getVerticesMod()[i].pos = {
            m->mVertices[i].x,
            m->mVertices[i].y,
            m->mVertices[i].z
        };

        getVerticesMod()[i].color = { 1, 1, 1 };

        getVerticesMod()[i].normal = {
            m->mNormals[i].x,
            m->mNormals[i].y,
            m->mNormals[i].z
        };

        if (m->HasTextureCoords(0)) {
            getVerticesMod()[i].uv = {
                m->mTextureCoords[0][i].x,
                m->mTextureCoords[0][i].y
            };
        }
        else {
            getVerticesMod()[i].uv = { 0.0f, 0.0f };
        }
    }

    getIndicesMod().clear();
    getIndicesMod().reserve(m->mNumFaces * 3);

    for (UINT i = 0; i < m->mNumFaces; ++i) {
        const aiFace& face = m->mFaces[i];

        if (face.mNumIndices != 3) {
            continue;
        }

        getIndicesMod().push_back(face.mIndices[0]);
        getIndicesMod().push_back(face.mIndices[1]);
        getIndicesMod().push_back(face.mIndices[2]);
    }

    indexCount = static_cast<UINT>(getIndicesMod().size());

    if (indexCount == 0) {
        CreateError("Mesh has no indices");
    }

    D3D11_BUFFER_DESC vbd{};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.ByteWidth = sizeof(Vertex) * static_cast<UINT>(getIndicesMod().size());
    vbd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA vsd{};
    vsd.pSysMem = getVerticesMod().data();

    if (FAILED(p_meshStruct.device->CreateBuffer(&vbd, &vsd, &vb))) {
        CreateError("Failed to create Vertex buffer");
    }

    D3D11_BUFFER_DESC ibd{};
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.ByteWidth = sizeof(uint32_t) * indexCount;
    ibd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA isd{};
    isd.pSysMem = getIndicesMod().data();

    if (FAILED(p_meshStruct.device->CreateBuffer(&ibd, &isd, &ib))) {
        CreateError("Failed to create index buffer");
    }
}

void MeshBackend::Draw(MeshDrawStruct& p_meshDrawStruct) const {
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    p_meshDrawStruct.ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    p_meshDrawStruct.ctx->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    p_meshDrawStruct.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    p_meshDrawStruct.ctx->DrawIndexed(indexCount, 0, 0);
}
#endif