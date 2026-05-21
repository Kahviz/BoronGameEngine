#include "SaveProject.h"
#include <fstream>
#include "GLOBALS.h"
#include "Instance.h"
#include <filesystem>
#include <iostream>
#include <Object.h>
#include <Graphics/Graphics.h>

namespace fs = std::filesystem;

void SaveProject::Save(const std::vector<std::unique_ptr<Instance>>& Drawables)
{
    std::string path = savings + "\\" + ProjectName;

    if (fs::exists(path))
    {
        fs::remove_all(path);
    }

    fs::create_directories(path);

    std::ofstream file(path + "\\save.BGEproject");

    for (auto& Drawable : Drawables) {
        file << "-\n";
        file << "Name: " << Drawable->Name << "\n";
        file << "Anchored: " << Drawable->Anchored << "\n";
        file << "Size: " << Drawable->transform.Size << "\n";
        file << "Orientation: " << Drawable->transform.Orientation << "\n";
        file << "Position: " << Drawable->transform.Position << "\n";
        file << "Color: " << Drawable->color << "\n";
        file << "CanDraw" << Drawable->CanDraw() << "\n";
    }

    file.close();
}

Instance& AddAMesh(const std::string& Path, const std::string& Name,
    Vector3 pos, Vector3 Size, bool Selec, Window& window, std::vector<std::unique_ptr<Instance>>& Drawables)
{
    Transform transform;
    transform.Position = pos;
    transform.Size = Size;

    auto obj = std::make_unique<Object>(
        Name,
        1,
        Int3(
            static_cast<int>(160),
            static_cast<int>(160),
            static_cast<int>(160)
        ),
        Int3(168, 160, 160),
        Vector3(0, 0, 0),
        transform,
        true,
        std::make_shared<Mesh>()
    );


    obj->UniqueID = Index;

#if DIRECTX11 == 1
    obj->OBJmesh = Mesh::Load(assets + Path, window.GetGraphics().GetDevice());
#endif

#if VULKAN == 1
    obj.get()->OBJmesh = Mesh::Load(
        assets + Path,
        window.GetGraphics().GetDevice(),
        window.GetGraphics().GetPhysicalDevice(),
        window.GetGraphics().VR.get()->GetCommandPool(),
        window.GetGraphics().VR.get()->GetGraphicsQueue()
    );
#endif

    obj->Selected = Selec;

    std::string fullPath = textures + "\\TestTexture.png";
    std::cout << fullPath << std::endl;

#if DIRECTX11 == 1
    obj->texture.Load(fullPath, *window.GetGraphics().DR.get());
#endif
#if VULKAN == 1
    obj->texture.LoadVK(fullPath, *window.GetGraphics().VR.get());
    window.GetGraphics().VR->UpdateDescriptorSet(obj.get()); //Updates DescriptorSets so the texture is loaded in the renderer

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

std::vector<std::unique_ptr<Instance>> SaveProject::Load(Window& window)
{
    std::vector<std::unique_ptr<Instance>> Loaded;
    std::ifstream file(savings + "\\" + ProjectName + "\\save.BGEproject");

    if (!file.is_open()) {
        std::cout << "File not found\n";
        return {};
    }

    std::string line;
    Vector3 loadedPos(0, 0, 0);

    while (std::getline(file, line))
    {
        if (line.rfind("Position:", 0) == 0)
        {
            std::stringstream ss(line.substr(10));

            ss >> loadedPos.x()
                >> loadedPos.y()
                >> loadedPos.z();


            AddAMesh(
                "\\Cube.obj",
                "LoadedObject", 
                loadedPos,
                Vector3(1, 1, 1),
                false,
                window,
                Loaded
            );
        }
    }

    return Loaded;
}