#include "SaveProject.h"
#include <fstream>
#include "GLOBALS.h"
#include "Instance.h"
#include <filesystem>
#include <iostream>
#include <Object.h>
#include <Graphics/Graphics.h>
#include <filesystem>

namespace fs = std::filesystem;

void SaveProject::Save(const std::vector<std::unique_ptr<Instance>>& Drawables)
{
    std::string path = savings + "\\" + g_projectName;
    std::string meshFilesPath = savings + "\\" + g_projectName + "\\MeshFiles";

    fs::create_directories(meshFilesPath);

    std::ofstream file(path + "\\save.BGEproject");

    for (auto& Drawable : Drawables) {
        fs::path from = Drawable->OBJmesh->GetMeshPath();

        std::string newName = std::to_string(Drawable->UniqueID);

        fs::path to =
            fs::path(meshFilesPath) /
            (newName + from.extension().string());

        if (from.lexically_normal() != to.lexically_normal())
        {
            fs::copy_file(
                from,
                to,
                fs::copy_options::overwrite_existing
            );
        }

        file << "-\n";
        file << "Name: " << Drawable->Name << "\n";
        file << "Anchored: " << Drawable->Anchored << "\n";
        file << "Size: " << Drawable->transform.Size << "\n";
        file << "Orientation: " << Drawable->transform.Orientation << "\n";
        file << "Position: " << Drawable->transform.Position << "\n";
        file << "Color: " << Drawable->color << "\n";
        file << "CanDraw: " << Drawable->CanDraw() << "\n";
        file << "UniqueID: " << Drawable->UniqueID << "\n";
        file << "ParentID: " << Drawable->Parent->UniqueID << "\n";
        file << "MeshFile: " << to.filename().string() << "\n";
        file << "END\n";
    }

    file.close();
}

Instance& AddAMesh(const std::string& Path,const int UniqueID, const std::string& Name,
    BML::Vector3 pos, BML::Vector3 Size, bool Selec, Window& window, std::vector<std::unique_ptr<Instance>>& Drawables)
{
    Transform transform;
    transform.Position = pos;
    transform.Size = Size;

    auto obj = std::make_unique<Object>(
        Name,
        1,
        BML::Int3(
            static_cast<int>(160),
            static_cast<int>(160),
            static_cast<int>(160)
        ),
        BML::Int3(168, 160, 160),
        BML::Vector3(0, 0, 0),
        transform,
        true,
        std::make_shared<Mesh>()
    );


    obj->UniqueID = UniqueID;
    std::string finalPath;

    if (fs::path(Path).is_absolute())
    {
        finalPath = Path;
    }
    else
    {
        finalPath = assets + Path;
    }

#if DIRECTX11 == 1
    obj->OBJmesh = Mesh::Load(finalPath, window.GetGraphics().GetDevice());
#endif

#if VULKAN == 1
    auto& gfx = window.GetGraphics();
    auto& vk = static_cast<VulkanAdapter&>(gfx.GetRenderer());

    obj.get()->OBJmesh = Mesh::Load(
        finalPath,
        vk.GetDevice(),
        vk.GetPhysicalDevice(),
        vk.GetCommandPool(),
        vk.GetGraphicsQueue()
    );
#endif

    obj->Selected = Selec;

    std::string fullPath = textures + "\\TestTexture.png";
    std::cout << fullPath << std::endl;

#if DIRECTX11 == 1
    obj->texture.Load(fullPath, window.GetGraphics().GetRenderer());
#endif
#if VULKAN == 1
    obj->texture.LoadVK(fullPath, vk);
    vk.UpdateDescriptorSet(obj.get()); //Updates DescriptorSets so the texture is loaded in the renderer

    /*
    if (Name != "Cube2") {

    }
    else {
        MakeAError("Making so Cube2 doesnt have texture for showcase! This is a remainder for me! 1F1G");
    }
    */

#endif
    Instance* objPtr = obj.get();

    Drawables.push_back(std::move(obj));

    Index++;
    return *objPtr;
}

std::vector<std::unique_ptr<Instance>> SaveProject::Load(Window& window,Instance& world)
{
    std::vector<std::unique_ptr<Instance>> Loaded;
    std::ifstream file(savings + "\\" + g_projectName + "\\save.BGEproject");

    if (!file.is_open()) {
        std::cout << "File not found\n";
        return {};
    }

    std::string line;

    std::string loadedName = "LoadedObject";
    BML::Vector3 loadedPos(0, 0, 0);
    BML::Vector3 loadedSize(1, 1, 1);
    BML::Vector3 loadedOrientation(0, 0, 0);
    std::string loadedMeshFile = "";
    std::string loadedUniqueID = "0";

    while (std::getline(file, line))
    {
        if (line == "-")
        {
            continue;
        }

        if (line.rfind("Name:", 0) == 0)
        {
            loadedName = line.substr(6);
        }

        else if (line.rfind("Position:", 0) == 0)
        {
            std::string data = line.substr(10);

            std::replace(data.begin(), data.end(), ',', ' ');

            std::stringstream ss(data);

            ss >> loadedPos.x()
                >> loadedPos.y()
                >> loadedPos.z();
        }

        else if (line.rfind("Size:", 0) == 0)
        {
            std::string data = line.substr(6);

            std::replace(data.begin(), data.end(), ',', ' ');

            std::stringstream ss(data);

            ss >> loadedSize.x()
                >> loadedSize.y()
                >> loadedSize.z();
            std::cout << loadedSize << std::endl;
        }
        else if (line.rfind("Orientation:", 0) == 0)
        {
            std::string data = line.substr(13);

            std::replace(data.begin(), data.end(), ',', ' ');

            std::stringstream ss(data);

            loadedOrientation = BML::Vector3(
                0, 0, 0
            );

            ss >> loadedOrientation.x()
                >> loadedOrientation.y()
                >> loadedOrientation.z();
        }
        else if (line.rfind("MeshFile:", 0) == 0)
        {
            loadedMeshFile = line.substr(10);
        }
        else if (line.rfind("UniqueID:", 0) == 0)
        {
            loadedUniqueID = line.substr(10);
        }
        else if (line == "END")
        {
            std::string meshPath =
                savings + "\\" +
                g_projectName +
                "\\MeshFiles\\" +
                loadedMeshFile;

            AddAMesh(
                meshPath,
                std::stoi(loadedUniqueID),
                loadedName,
                loadedPos,
                loadedSize,
                false,
                window,
                Loaded
            );
        }
    }

    return Loaded;
}