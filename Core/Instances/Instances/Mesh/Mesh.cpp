#include "Mesh.h"

#if DIRECTX11 == 1
    #include "DX11/MeshDX11.h"
#endif

#if VULKAN == 1
    #include "Vulkan/MeshVulkan.h"
#endif

std::shared_ptr<Mesh> Mesh::Load(const fs::path& p_file, MeshStruct& p_meshStruct) {
    static std::unordered_map<fs::path, std::shared_ptr<Mesh>> Cache;

    const auto path = p_file.lexically_normal();

    auto it = Cache.find(path);

    if (it != Cache.end()) {
        return it->second;
    }

    auto mesh = std::make_shared<MeshBackend>();
    mesh->LoadMesh(p_file, p_meshStruct);

    mesh->getMeshPath() = p_file;

    std::string name = p_file.filename().string();
    mesh->getMeshFileName() = name;

    Cache.emplace(p_file, mesh);

    return mesh;
}
