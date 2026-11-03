#pragma once

#include "GLOBALS.h"
#include "BoronMathLibrary.h"

#include "GraphicsBackends.h"

#if DIRECTX11 == 1
    struct PerFrameStuct {
        ID3D11RenderTargetView* renderTargetView = nullptr;
    };

    struct BoronGuiNeeds {
        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* deviceContext = nullptr;

        IDXGISwapChain* swapChain = nullptr;
    };
#endif

#if VULKAN == 1
struct PerFrameStuct {
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

struct BoronGuiNeeds {
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkExtent2D swapchainExtent{};
};
#endif