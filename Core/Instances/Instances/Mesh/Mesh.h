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
            it->second->GetIsCached() = true;
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->GetMeshPath() = file;

        std::string name = std::filesystem::path(file).filename().string();

        mesh->GetMeshFileName() = name;

        mesh->DM.Load(file, device);

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
            it->second->GetIsCached() = true;
            return it->second;
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->GetMeshPath() = file;

        std::string name = std::filesystem::path(file).filename().string();

        mesh->GetMeshFileName() = name;

        mesh->VM.Load(file, device, phyDevice, cmdPool, gfxQueue);

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
        #endif

        #if DIRECTX11 == 1
            return DM.GetIndices();
        #endif
    }

    const std::vector<Vertex>& GetVertices() const {
        #if VULKAN == 1
            return VM.GetVertices();
        #endif

        #if DIRECTX11 == 1
            return DM.GetVertices();
        #endif
    }
    #if VULKAN == 1
        MeshVK VM;
    #endif

    #if DIRECTX11 == 1
        MeshDX11 DM;
    #endif

    std::string& GetMeshFileName() {
        return MeshFileName;
    }

    std::string& GetMeshPath() {
        return MeshPath;
    }

    bool& GetIsCached() {
        return cached;
    }
private:
    std::string MeshPath = "NULL";
    std::string MeshFileName = "NULL";
    bool cached = false;
};