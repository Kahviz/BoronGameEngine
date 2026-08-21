#include "SaveProject.h"
#include <fstream>
#include "GLOBALS.h"
#include "Instance.h"
#include <filesystem>
#include <iostream>
#include <Object.h>
#include <Graphics/Graphics.h>
#include <Script.h>

namespace fs = std::filesystem;

void SaveProject::Save(ECS& ecs)
{
    std::string path = savings + "\\" + g_projectName;
    std::string meshFilesPath = savings + "\\" + g_projectName + "\\MeshFiles";

    fs::create_directories(meshFilesPath);

    std::ofstream file(path + "\\save.BGEproject");

    ecs.Each<
        BasicInfoComponent,
        TransformComponent,
        PhysicsComponent,
        ObjectComponent,
        HierarcyComponent,
        InstanceTypeComponent,
        ColorComponent
    >(
        [&](EntityECS entity,
            BasicInfoComponent& basic,
            TransformComponent& transform,
            PhysicsComponent& physics,
            ObjectComponent& object,
            HierarcyComponent& hierarchy,
            InstanceTypeComponent& instanceType,
            ColorComponent colorComp
            )
        {
            fs::path from = object.OBJmesh->GetMeshPath();

            std::string newName = std::to_string(entity);

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

            file << "Name: " << basic.Name << "\n";
            file << "Anchored: " << physics.anchored << "\n";

            file << "Size: "
                << transform.transform.Size << "\n";

            file << "Orientation: "
                << transform.transform.Orientation << "\n";

            file << "Position: "
                << transform.transform.Position << "\n";

            file << "Color: "
                << colorComp.color << "\n";

            file << "CanDraw: "
                << object.canDraw << "\n";

            file << "UniqueID: "
                << entity << "\n";

            if (hierarchy.parent != 0)
                file << "ParentID: "
                << hierarchy.parent << "\n";
            else
                file << "ParentID: -1\n";

            file << "MeshFile: "
                << to.filename().string() << "\n";

            file << "InstanceType: "
                << static_cast<int>(instanceType.InstanceType)
                << '\n';

            file << "END\n";
        }
    );

    file.close();
}

EntityECS AddAMesh(ECS& ecs,EntityECS world, Window& window, const std::string& Path, const std::string& Name,
    BML::Vector3 pos, BML::Vector3 Size,BML::Int3 color, bool Selec, bool LiteralPath, bool UsesTexture)
{
    Transform transform;
    transform.Position = pos;
    transform.Size = Size;

    EntityECS entity = ecs.createEntity();

    BasicInfoComponent basicComp;
    ColorComponent colorComp;
    TransformComponent transformComp;
    PhysicsComponent physicsComp;
    ObjectComponent objectComp;
    EditorSettingsComponent editorComp;
    HierarcyComponent hierarcyComp;
    TextureComponent textureComp;
    InstanceTypeComponent instTypeComp;
    instTypeComp.InstanceType = Boron::Enums::InstanceType::Object;

    editorComp.isVisibleInExplorer = true;
    basicComp.Name = Name;
    colorComp.color = color;
        
    transformComp.transform = transform;
    physicsComp.anchored = true;

#if DIRECTX11 == 1
    if (!LiteralPath) {
        obj->OBJmesh = Mesh::Load(assets + Path, window.GetGraphics().GetDevice());
    }
    else {
        obj->OBJmesh = Mesh::Load(Path, window.GetGraphics().GetDevice());
    }
#endif

#if VULKAN == 1
    auto& vk = static_cast<VulkanAdapter&>(window.GetGraphics().GetRenderer());

    if (!LiteralPath) {
        objectComp.OBJmesh = Mesh::Load(
            assets + Path,
            vk.GetDevice(),
            vk.GetPhysicalDevice(),
            vk.GetCommandPool(),
            vk.GetGraphicsQueue()
        );
    }
    else {
        objectComp.OBJmesh = Mesh::Load(
            Path,
            vk.GetDevice(),
            vk.GetPhysicalDevice(),
            vk.GetCommandPool(),
            vk.GetGraphicsQueue()
        );
    }

#endif

    std::string fullPath = textures + "\\TestTexture.png";

    CreateError("No texture here for now");

    hierarcyComp.parent = world;

    ecs.AddComponent(entity, basicComp);
    ecs.AddComponent(entity, colorComp);
    ecs.AddComponent(entity, transformComp);
    ecs.AddComponent(entity, physicsComp);
    ecs.AddComponent(entity, objectComp);
    ecs.AddComponent(entity, hierarcyComp);
    ecs.AddComponent(entity, editorComp);
    ecs.AddComponent(entity, textureComp);
    ecs.AddComponent(entity, instTypeComp);

    return entity;
}


void SaveProject::Load(ECS& ecs, Window& window, EntityECS world)
{
    std::ifstream file(
        savings + "\\" + g_projectName + "\\save.BGEproject"
    );

    if (!file.is_open())
    {
        std::cout << "File not found\n";
        return;
    }

    struct PendingParent
    {
        EntityECS child;
        EntityECS oldParent;
    };

    std::vector<PendingParent> pendingParents;

    std::unordered_map<EntityECS, EntityECS> entityIDMap;

    std::string line;

    std::string loadedName;
    BML::Vector3 loadedPos(0, 0, 0);
    BML::Vector3 loadedSize(1, 1, 1);
    BML::Vector3 loadedOrientation(0, 0, 0);

    std::string loadedMeshFile;

    EntityECS loadedUniqueID = 0;
    EntityECS loadedParentID = static_cast<EntityECS>(-1);

    BML::Int3 loadedColor(255, 255, 255);

    bool loadedAnchored = true;
    bool loadedCanDraw = true;

    Boron::Enums::InstanceType loadedInstanceType =
        Boron::Enums::InstanceType::Object;

    while (std::getline(file, line))
    {
        if (line == "-")
            continue;

        if (line.rfind("Name:", 0) == 0)
        {
            loadedName = line.substr(6);
        }

        else if (line.rfind("Position:", 0) == 0)
        {
            std::string data = line.substr(10);

            std::replace(
                data.begin(),
                data.end(),
                ',',
                ' '
            );

            std::stringstream ss(data);

            ss >> loadedPos.x()
                >> loadedPos.y()
                >> loadedPos.z();
        }

        else if (line.rfind("Size:", 0) == 0)
        {
            std::string data = line.substr(6);

            std::replace(
                data.begin(),
                data.end(),
                ',',
                ' '
            );

            std::stringstream ss(data);

            ss >> loadedSize.x()
                >> loadedSize.y()
                >> loadedSize.z();
        }

        else if (line.rfind("Orientation:", 0) == 0)
        {
            std::string data = line.substr(13);

            std::replace(
                data.begin(),
                data.end(),
                ',',
                ' '
            );

            std::stringstream ss(data);

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
            loadedUniqueID =
                static_cast<EntityECS>(
                    std::stoul(line.substr(10))
                    );
        }

        else if (line.rfind("Color:", 0) == 0)
        {
            std::string data = line.substr(6);

            std::replace(
                data.begin(),
                data.end(),
                ',',
                ' '
            );

            std::stringstream ss(data);

            int r, g, b;

            ss >> r >> g >> b;

            loadedColor = BML::Int3(r, g, b);
        }

        else if (line.rfind("Anchored:", 0) == 0)
        {
            loadedAnchored =
                std::stoi(line.substr(9)) != 0;
        }

        else if (line.rfind("CanDraw:", 0) == 0)
        {
            loadedCanDraw =
                std::stoi(line.substr(8)) != 0;
        }

        else if (line.rfind("ParentID:", 0) == 0)
        {
            int parentID = std::stoi(line.substr(10));

            if (parentID == -1)
            {
                loadedParentID =
                    static_cast<EntityECS>(-1);
            }
            else
            {
                loadedParentID =
                    static_cast<EntityECS>(parentID);
            }
        }

        else if (line.rfind("InstanceType:", 0) == 0)
        {
            int value = std::stoi(line.substr(14));

            loadedInstanceType =
                static_cast<Boron::Enums::InstanceType>(value);
        }

        else if (line == "END")
        {
            if (loadedInstanceType !=
                Boron::Enums::InstanceType::Object)
            {
                std::cerr
                    << "Unsupported InstanceType: "
                    << static_cast<int>(loadedInstanceType)
                    << '\n';

                loadedName.clear();
                loadedMeshFile.clear();
                loadedUniqueID = 0;
                loadedParentID = static_cast<EntityECS>(-1);

                continue;
            }

            std::string meshPath =
                savings + "\\" +
                g_projectName +
                "\\MeshFiles\\" +
                loadedMeshFile;

            if (!fs::exists(meshPath))
            {
                std::cerr
                    << "Mesh does not exist: "
                    << meshPath
                    << '\n';

                continue;
            }

            EntityECS entity = AddAMesh(
                ecs,
                world,
                window,
                meshPath,
                loadedName,
                loadedPos,
                loadedSize,
                loadedColor,
                false,
                true,
                true
            );

            if (entity == 0)
                continue;

            entityIDMap[loadedUniqueID] = entity;

            auto& transform =
                ecs.GetComponent<TransformComponent>(entity);

            transform.transform.Orientation =
                loadedOrientation;

            auto& physics =
                ecs.GetComponent<PhysicsComponent>(entity);

            physics.anchored =
                loadedAnchored;

            auto& object =
                ecs.GetComponent<ObjectComponent>(entity);

            object.canDraw =
                loadedCanDraw;

            if (loadedParentID !=
                static_cast<EntityECS>(-1))
            {
                pendingParents.push_back({
                    entity,
                    loadedParentID
                    });
            }

            loadedName.clear();
            loadedMeshFile.clear();

            loadedPos = { 0, 0, 0 };
            loadedSize = { 1, 1, 1 };
            loadedOrientation = { 0, 0, 0 };

            loadedUniqueID = 0;
            loadedParentID =
                static_cast<EntityECS>(-1);

            loadedColor = { 255, 255, 255 };

            loadedAnchored = true;
            loadedCanDraw = true;

            loadedInstanceType =
                Boron::Enums::InstanceType::Object;
        }
    }

    file.close();

    for (const PendingParent& pending : pendingParents)
    {
        auto childIt =
            entityIDMap.find(
                pending.child
            );

        if (childIt == entityIDMap.end())
            continue;

        auto parentIt =
            entityIDMap.find(
                pending.oldParent
            );

        if (parentIt == entityIDMap.end())
        {
            ecs.GetComponent<HierarcyComponent>(
                childIt->second
            ).parent = world;

            continue;
        }

        EntityECS child =
            childIt->second;

        EntityECS parent =
            parentIt->second;

        ecs.GetComponent<HierarcyComponent>(
            child
        ).parent = parent;
    }
}