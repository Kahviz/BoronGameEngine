#pragma once

#include <vector>
#include <memory>

#include "GLOBALS.h"
#include "Vulkan.h"
#include "Mesh/Mesh.h"

#if VULKAN == 1
class MeshBackend : public Mesh
{
public:
    void LoadMesh(const fs::path& file, MeshStruct& p_meshStruct);
    void Draw(MeshDrawStruct& p_meshDrawStruct) const override;

    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
private:
    VkDeviceMemory vertexMemory = VK_NULL_HANDLE;

    VkDeviceMemory indexMemory = VK_NULL_HANDLE;

    uint32_t indexCount = 0;
};
#endif