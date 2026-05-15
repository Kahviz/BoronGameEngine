#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include "GLOBALS.h"

#include "DX11/MeshDX11.h"
#include "Vulkan/MeshVulkan.h"
#include "BoronMathLibrary.h"
#include <Instances/Vertex.h>
#include <unordered_map>
#include <memory>
#include "ErrorHandling/ErrorMessage.h"

class Mesh
{
public:
#if DIRECTX11 == 1
    static std::shared_ptr<Mesh> Load(const std::string& file, ID3D11Device* device) {
        static std::unordered_map<std::string, std::shared_ptr<Mesh>> Cache;

        auto it = Cache.find(file);
        if (it != Cache.end())
        {
            MakeAError("Caching...");
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>();

        mesh->DM.Load(file, device);

        MakeAError("Not using Caching...");

        Cache.emplace(file, mesh);

        return mesh;
    }
    void DrawForDX11(ID3D11DeviceContext* ctx) const {
        DM.Draw(ctx);
    };
#endif
    
#if VULKAN == 1
    static std::shared_ptr<Mesh> Load(
        const std::string& file,
        VkDevice device,
        VkPhysicalDevice phyDevice,
        VkCommandPool cmdPool,
        VkQueue gfxQueue
    )
    {
        static std::unordered_map<std::string, std::shared_ptr<Mesh>> Cache;

        auto it = Cache.find(file);
        if (it != Cache.end())
        {
            MakeAError("Caching...");
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>();

        mesh->VM.Load(file, device, phyDevice, cmdPool, gfxQueue);

        MakeAError("Not using Caching...");

        Cache.emplace(file, mesh);

        return mesh;
    }

    void DrawForVulkan(VkCommandBuffer cb)
    {
        VM.Draw(cb);
    }

#endif

    const std::vector<uint32_t>& GetIndices() const {
        #if VULKAN == 1
            return VM.GetIndices();
        #else
            return DM.GetIndices();
        #endif
    }

    const std::vector<Vertex>& GetVertices() const {
        #if VULKAN == 1
            return VM.GetVertices();
        #else
            return DM.GetVertices();
        #endif
    }
    #if VULKAN == 1
        MeshVK VM;
    #endif

    #if DIRECTX11 == 1
        MeshDX11 DM;
    #endif
};
