#pragma once

#include <vector>
#include <Instances/Vertex.h>
#include "GLOBALS.h"
#include "Vulkan.h"

#if VULKAN == 1
class MeshVK
{
public:
    void Load(const fs::path& file, VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue);

    void Draw(VkCommandBuffer cmd) const;

    const std::vector<uint32_t>& GetIndices() const {
        return indices;
    }

    const std::vector<Vertex>& GetVertices() const {
        return verts;
    }

    std::vector<Vertex> verts;
    std::vector<uint32_t> indices;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
private:
    VkDeviceMemory vertexMemory = VK_NULL_HANDLE;

    VkDeviceMemory indexMemory = VK_NULL_HANDLE;

    uint32_t indexCount = 0;
};
#endif