#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "GLOBALS.h"

#include "BoronMathLibrary.h"
#include "Instances/Vertex.h"

#include "Logger/Logger.h"

#include "Vulkan.h"
#include "DX11.h"

class Mesh
{
public:
    virtual void Draw(MeshDrawStruct& p_meshDrawStruct) const = 0;

    static std::shared_ptr<Mesh> Load(const fs::path& p_file, MeshStruct& p_meshStruct);

    std::string& getMeshFileName() {
        return m_meshFileName;
    }

    fs::path& getMeshPath() {
        return m_meshPath;
    }

    bool& getIsCached() {
        return m_cached;
    }

    const std::vector<uint32_t>& getIndices() const {
        return m_indices;
    }

    const std::vector<Vertex>& getVertices() const {
        return m_verts;
    }

    std::vector<uint32_t>& getIndicesMod() {
        return m_indices;
    }

    std::vector<Vertex>& getVerticesMod() {
        return m_verts;
    }
private:
    fs::path m_meshPath = "NULL";
    std::string m_meshFileName = "NULL";
    bool m_cached = false;

    std::vector<Vertex> m_verts{};
    std::vector<uint32_t> m_indices{};
};