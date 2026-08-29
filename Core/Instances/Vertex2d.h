#pragma once
#include <array>
#include "BoronMathLibrary.h"
#include "GLOBALS.h"

#if VULKAN == 1
#include "Vulkan/vulkan.h"

struct Vertex2d
{
    GPUVector2 pos = { 0,0,0 };
    GPUVector3 color = { 0,0,0 };
    GPUVector2 uv = { 0,0 };
    float brightness = 1.0f;

    Vertex2d() = default;

    Vertex2d(float b, const GPUVector2& p, const GPUVector3& c, const GPUVector3& n)
        : brightness(b), pos(p), color(c), uv{ 0.0f, 0.0f } {
    }

    Vertex2d(float b, const GPUVector2& p, const GPUVector3& c, const GPUVector3& n, const GPUVector2& uv_coords)
        : brightness(b), pos(p), color(c), uv(uv_coords) {
    }

    Vertex2d(const GPUVector2& p,const GPUVector3& c)
        : pos(p),
        color(c),
        uv{ 0.0f, 0.0f },
        brightness(1.0f)
    {
    }

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex2d);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex2d, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex2d, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex2d, uv);

        return attributeDescriptions;
    }
};
#endif