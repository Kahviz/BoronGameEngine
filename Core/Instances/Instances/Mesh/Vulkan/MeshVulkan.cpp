#include "MeshVulkan.h"

#include "GLOBALS.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include "Vulkan/VulkanHelpers.h"
#include "Logger/Logger.h"

#if VULKAN == 1
void MeshBackend::LoadMesh(const fs::path& file, MeshStruct& p_meshStruct) {
    Assimp::Importer imp{};
    const aiScene* scene = imp.ReadFile(
        file.string(),
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_OptimizeMeshes
    );

    if (!scene || !scene->HasMeshes()) {
        CreateError("Failed to load model: ", imp.GetErrorString());
    }
    
    aiMesh* m = scene->mMeshes[0];
    if (!m || m->mNumVertices == 0) {
        CreateError("Invalid mesh data");
    }

    if (!m->HasNormals())
        throw std::runtime_error("Mesh has no normals");

    getVerticesMod().resize(m->mNumVertices);
    for (uint32_t i = 0; i < m->mNumVertices; ++i)
    {
        getVerticesMod()[i].brightness = 1.0f;

        getVerticesMod()[i].pos = {
            m->mVertices[i].x,
            m->mVertices[i].y,
            m->mVertices[i].z
        };

        getVerticesMod()[i].normal = {
            m->mNormals[i].x,
            m->mNormals[i].y,
            m->mNormals[i].z
        };

        getVerticesMod()[i].color = { 1, 1, 1 };
        if (m->HasTextureCoords(0)) {
            getVerticesMod()[i].uv = {
                m->mTextureCoords[0][i].x,
                m->mTextureCoords[0][i].y
            };
        }
        else {
            CreateError("No uv:s!");
            getVerticesMod()[i].uv = { 0.0f, 0.0f };
        }
    }

    getIndicesMod().reserve(m->mNumFaces * 3);
    for (uint32_t i = 0; i < m->mNumFaces; ++i) {
        const aiFace& face = m->mFaces[i];
        if (face.mNumIndices != 3)
            continue;

        getIndicesMod().push_back(face.mIndices[0]);
        getIndicesMod().push_back(face.mIndices[1]);
        getIndicesMod().push_back(face.mIndices[2]);
    }

    indexCount = static_cast<uint32_t>(getIndicesMod().size());
    if (indexCount == 0)
        throw std::runtime_error("Mesh has no indices");

    VkDeviceSize vSize = sizeof(Vertex) * getVertices().size();
    VkDeviceSize iSize = sizeof(uint32_t) * getIndices().size();

    //Staging buffers
    VkBuffer vStaging, iStaging;
    VkDeviceMemory vStagingMem, iStagingMem;

    CreateBuffer(
        vSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        vStaging,
        vStagingMem,
        p_meshStruct.device, p_meshStruct.physicalDevice
    );

    CreateBuffer(
        iSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        iStaging,
        iStagingMem,
        p_meshStruct.device,
        p_meshStruct.physicalDevice
    );

    void* data;
    vkMapMemory(p_meshStruct.device, vStagingMem, 0, vSize, 0, &data);
    memcpy(data, getVertices().data(), (size_t)vSize);
    vkUnmapMemory(p_meshStruct.device, vStagingMem);

    vkMapMemory(p_meshStruct.device, iStagingMem, 0, iSize, 0, &data);
    memcpy(data, getIndices().data(), (size_t)iSize);
    vkUnmapMemory(p_meshStruct.device, iStagingMem);

    CreateBuffer(
        vSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vertexBuffer,
        vertexMemory,
        p_meshStruct.device,
        p_meshStruct.physicalDevice
    );

    CreateBuffer(
        iSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        indexBuffer,
        indexMemory,
        p_meshStruct.device,
        p_meshStruct.physicalDevice
    );

    CopyBuffer(
        vStaging,        // src
        vertexBuffer,    // dst
        vSize,           // size
        p_meshStruct.commandPool,     // cmdp
        p_meshStruct.device,          // device
        p_meshStruct.graphicsQueue    // gQ
    );

    CopyBuffer(
        iStaging,
        indexBuffer,
        iSize,
        p_meshStruct.commandPool,
        p_meshStruct.device,
        p_meshStruct.graphicsQueue
    );


    vkDestroyBuffer(p_meshStruct.device, vStaging, nullptr);
    vkFreeMemory(p_meshStruct.device, vStagingMem, nullptr);

    vkDestroyBuffer(p_meshStruct.device, iStaging, nullptr);
    vkFreeMemory(p_meshStruct.device, iStagingMem, nullptr);
}

void MeshBackend::Draw(MeshDrawStruct& p_meshDrawStruct) const {
    VkBuffer vbs[] = { vertexBuffer };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(p_meshDrawStruct.commandBuffer, 0, 1, vbs, offsets);
    vkCmdBindIndexBuffer(p_meshDrawStruct.commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(p_meshDrawStruct.commandBuffer, indexCount, 1, 0, 0, 0);
}
#endif
