#include "Mesh.h"
#include "Vulkan/MeshVulkan.h"
#include "DX11/MeshDX11.h"

std::shared_ptr<Mesh> Mesh::Load(const fs::path& p_file, MeshStruct& p_meshStruct) {
    static std::unordered_map<fs::path, std::shared_ptr<Mesh>> Cache;

    auto it = Cache.find(p_file);
    if (it != Cache.end()) {
        it->second->getIsCached() = true;
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
