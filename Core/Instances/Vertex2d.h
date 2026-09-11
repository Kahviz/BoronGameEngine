#pragma once

#include <array>
#include "BoronMathLibrary.h"
#include "GLOBALS.h"

#include "Vulkan.h"

struct Vertex2d {
    GPUVector2 pos = { 0,0 };
    GPUVector4 color = { 0,0,0 };
    GPUVector2 uv = { 0,0 };
    GPUVector2 size = { 0,0 };
    GPUVector2 guiPos = { 0,0 };

    float brightness = 0.0f;
    float rounding = 10.0f;

    Vertex2d() = default;

    Vertex2d(float b, const GPUVector2& p, const GPUVector4& c, const GPUVector3& n)
        : brightness(b), pos(p), color(c), uv{ 0.0f, 0.0f }, size(100, 100), guiPos(100, 100) {
    }

    Vertex2d(float b, const GPUVector2& p, const GPUVector4& c, const GPUVector2& uv_coords)
        : brightness(b), pos(p), color(c), uv(uv_coords), size(100, 100), guiPos(200, 100) {
    }

    Vertex2d(const GPUVector2& p, const GPUVector4& c)
        : pos(p),
        color(c),
        uv{ 0.0f, 0.0f },
        brightness(1.0f),
        size(100, 100),
        guiPos(100,100)
    {
    }

    Vertex2d(const GPUVector2& p)
        : pos(p),
        color(1,0,0),
        uv{ 0.0f, 0.0f },
        brightness(1.0f),
        size(100, 100),
        guiPos(100, 100)
    {
    }

    #if VULKAN == 1
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex2d);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 7> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 7> attributes{};

            //pos
            attributes[0].binding = 0;
            attributes[0].location = 0;
            attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributes[0].offset = offsetof(Vertex2d, pos);

            // color
            attributes[1].binding = 0;
            attributes[1].location = 1;
            attributes[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributes[1].offset = offsetof(Vertex2d, color);

            // UV
            attributes[2].binding = 0;
            attributes[2].location = 2;
            attributes[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributes[2].offset = offsetof(Vertex2d, uv);

            // brightness
            attributes[3].binding = 0;
            attributes[3].location = 3;
            attributes[3].format = VK_FORMAT_R32_SFLOAT;
            attributes[3].offset = offsetof(Vertex2d, brightness);

            //size
            attributes[4].binding = 0;
            attributes[4].location = 4;
            attributes[4].format = VK_FORMAT_R32G32_SFLOAT;
            attributes[4].offset = offsetof(Vertex2d, size);

            //rounding
            attributes[5].binding = 0;
            attributes[5].location = 5;
            attributes[5].format = VK_FORMAT_R32_SFLOAT;
            attributes[5].offset = offsetof(Vertex2d, rounding);

            //guiPos
            attributes[6].binding = 0;
            attributes[6].location = 6;
            attributes[6].format = VK_FORMAT_R32G32_SFLOAT;
            attributes[6].offset = offsetof(Vertex2d, guiPos);

            return attributes;
        }
    #endif
};
