#include "MakeGui.h"
#include <algorithm>
#include "imgui.h"
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "GLOBALS.h"
#include "Graphics/Graphics.h"
#include <imgui_internal.h>
#include "Runtime/Engine.h"

struct MeshButton
{
    const char* label;
    std::string path;
    const char* name;
};

static bool InputTextStd(const char* label, std::string& str)
{
    char buf[256];
    strncpy_s(buf, str.c_str(), sizeof(buf));
    buf[255] = '\0';

    if (ImGui::InputText(label, buf, sizeof(buf)))
    {
        str = buf;
        return true;
    }
    return false;
}

static MeshButton meshButtons[] =
{
    { "Cube",     assets + "\\Cube.obj","Cube" },
    { "Ball",     assets + "\\Ball.obj",  "Ball" },
    { "Cylinder", assets + "\\Cylinder.obj", "Cylinder"}
};

void MakeChildrenNodes(Instance* inst) {
    if (!inst) return;

    std::vector<Instance*> children = inst->GetChildren();
    for (auto& child : children) {
        if (!child)
            continue;

        std::vector<Instance*> grandChildren = child->GetChildren();
        if (grandChildren.empty())
        {
            ImGui::TreeNodeEx((void*)child,
                ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen,
                "%s",
                child->Name.c_str()
            );
        }
        else
        {
            if (ImGui::TreeNodeEx((void*)child, ImGuiTreeNodeFlags_OpenOnArrow, "%s", child->Name.c_str()))
            {
                MakeChildrenNodes(child);
                ImGui::TreePop();
            }
        }
    }
}

BML::Vector3 MakeVec3TextEdit(Instance* inst,
    const std::string& name,
    const std::string& VecType)
{
    return { 0,0,0 };
}

void MakeGui::MakeStyle() {
    style.CreateImGuiStyle();
}

void MakeFloat3Edit(const char* Name, BML::Vector3& vec) {
    char label[128];
    static int Times = 0;
    Times += 1;

    snprintf(label, sizeof(label), "%s: ", Name);

    ImGui::Text("%s", label);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat("##x", &vec.x(), 0.0f, 0.0f, "%g", ImGuiInputTextFlags_CharsDecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat("##y", &vec.y(), 0.0f, 0.0f, "%g", ImGuiInputTextFlags_CharsDecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat("##z", &vec.z(), 0.0f, 0.0f, "%g", ImGuiInputTextFlags_CharsDecimal);
}

void MakeGui::MakeIMGui(Window& wnd,
    std::vector<std::unique_ptr<Instance>>& Drawables,
    std::function<Instance*
    (
        const std::string&,
        const std::string&,
        BML::Vector3,
        BML::Vector3,
        bool
        )>
    AddAMesh,
    float* Color3,
    bool Selec,
    Engine* engine,
    Instance& world
)
{ 
    MakeStyle();
    
    if (ImGui::IsAnyItemActive()) {
        Typing = true;
    }
    else {
        Typing = false;
    }
    
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = 0;

    static bool Switch = false;
    static bool CanChange = true;

    static ImVec2 LastSize2;
    static ImVec2 LastSize1;

    if (!Switch) {
        LastSize1 = io.DisplaySize;
        Switch = true;

        if (LastSize1.x != LastSize2.x || LastSize1.y != LastSize2.y) {
            CanChange = true;
        }
    }
    else {
        LastSize2 = io.DisplaySize;
        Switch = false;

        if (LastSize1.x != LastSize2.x || LastSize1.y != LastSize2.y) {
            CanChange = true;
        }
    }

    float screen_w = io.DisplaySize.x;
    float screen_h = io.DisplaySize.y;

    float window_w = screen_w / 5.0f;
    float window_h = screen_h / 2.0f;

    if (CanChange) {
        ImGui::SetNextWindowSize(ImVec2(window_w * 1.5f, window_h / 1.7f));

        float desiredX = screen_w - window_w * 1.5f;
        float desiredY = screen_h - screen_h / 3.45f;

        float finalX = ImClamp(desiredX, 0.0f, screen_w - window_w * 1.5f);
        float finalY = ImClamp(desiredY, 0.0f, screen_h - window_h / 2.0f);

        ImGui::SetNextWindowPos(ImVec2(finalX, finalY));
    }

    ImGui::Begin("Properties", nullptr,
        ImGuiWindowFlags_AlwaysHorizontalScrollbar |
        ImGuiWindowFlags_AlwaysVerticalScrollbar
    );
    
    if (world.Selected) {
        for (const auto& Drawable : Drawables) {
            Drawable.get()->Selected = false;
        }

        ImGui::Text("Name: ");
        ImGui::SameLine();
        InputTextStd("##NameWORLD", world.Name);
    }

    for (const auto& Drawable : Drawables) {
        if (Drawable.get()->Selected) {
            world.Selected = false;

            Instance& inst = *Drawable.get();

            //Name
            ImGui::Text("Name: ");
            ImGui::SameLine();
            InputTextStd("##NameWORLD", world.Name);

            //Pos
            MakeFloat3Edit("Position", inst.transform.Position);

            //Anchored
            ImGui::Checkbox("Anchored: ", &inst.Anchored);
            //Size
            //MakeFloat3Edit("Size", inst.Size); UNIQUE ID
        }
    }

    ImGui::End();

    if (CanChange) {
        CreateInfo("Changing");
        float windowWidth = window_w * 1.5f;
        float windowHeight = window_h / 1.0868f;

        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

        float desiredX = screen_w - window_w / 1.006f;
        float desiredY = screen_h / 4.0f;

        float finalX = ImClamp(desiredX, 0.0f, screen_w - windowWidth);
        float finalY = ImClamp(desiredY, 0.0f, screen_h - windowHeight);

        ImGui::SetNextWindowPos(ImVec2(finalX, finalY));
    }

    ImGui::Begin("Explorer", nullptr,
        ImGuiWindowFlags_AlwaysHorizontalScrollbar |
        ImGuiWindowFlags_AlwaysVerticalScrollbar
    );

    std::string WorldName = world.Name + " ##world";

    bool worldNodeOpen = ImGui::TreeNodeEx(WorldName.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

    if (ImGui::IsItemClicked()) {
        for (auto& i : Drawables) {
            i->Deselect();
        }
        world.Selected = true;
    }

    if (worldNodeOpen)
    {
        for (auto& instPtr : Drawables)
        {
            Instance* inst = instPtr.get();
            inst->Parent = &world;

            if (!inst || inst->Parent != &world)
                continue;

            if (inst->IsVisibleInExplorer) {
                if (ImGui::TreeNodeEx(
                    (void*)inst,
                    ImGuiTreeNodeFlags_OpenOnArrow,
                    "%s",
                    inst->Name.c_str()))
                {
                    MakeChildrenNodes(inst);
                    ImGui::TreePop();
                }
            }

            if (ImGui::IsItemClicked())
            {
                for (auto& i : Drawables)
                    i->Deselect();

                inst->Select();
                world.Selected = false;
            }
        }

        ImGui::TreePop();
    }

    ImGui::End();

    if (CanChange) {

        ImGui::SetNextWindowSize(ImVec2(screen_w, screen_h / 4.0f), ImGuiCond_Always);

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    }
    static bool openFileBrowser = false;
    static bool Downloads = false;
    static fs::path currentPath = fs::current_path();

    if (Downloads == false)
    {
        #ifdef _WIN32
            char* userProfile = nullptr;
            size_t len = 0;

            if (_dupenv_s(&userProfile, &len, "USERPROFILE") == 0 && userProfile)
            {
                currentPath = fs::path(userProfile) / "Downloads";
                free(userProfile);
            }
            else
            {
                currentPath = fs::current_path();
            }
        #else
            const char* home = std::getenv("HOME");

            if (home)
                currentPath = fs::path(home) / "Downloads";
            else
                currentPath = fs::current_path();
        #endif

        Downloads = true;
    }

    if (ImGui::Begin("BoronEngine", nullptr)) {
        if (ImGui::BeginTabBar("##TABS")) {
            if (ImGui::BeginTabItem("Home")) {
                if (ImGui::Button("Move",ImVec2(screen_w / 15,screen_h / 10))) {
                    for (auto& DrawablePTR : Drawables) {
                        if (DrawablePTR.get()->Selected) {
                            Instance* inst = DrawablePTR.get();

                            if (inst->IsVisibleInExplorer) {
                                //Move it makes arrow that cant be deleted by user...
                            }
                        }
                    }
                }

                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Import")) {
                if (ImGui::Button("Import From path", ImVec2(screen_w / 15, screen_h / 10))) {
                    openFileBrowser = true;
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    if (openFileBrowser)
    {
        ImGui::Begin("Select File", &openFileBrowser);

        ImGui::Text("%s", currentPath.string().c_str());

        if (currentPath.has_parent_path())
        {
            if (ImGui::Button(".."))
            {
                currentPath = currentPath.parent_path();
            }
        }

        for (const auto& entry : fs::directory_iterator(currentPath))
        {
            std::string name = entry.path().filename().string();

            if (entry.is_directory())
            {
                if (ImGui::Selectable((name + "/").c_str()))
                {
                    currentPath = entry.path();
                }
            }
            else
            {
                std::string ext = entry.path().extension().string();

                std::transform(
                    ext.begin(),
                    ext.end(),
                    ext.begin(),
                    [](unsigned char c) { return std::tolower(c); }
                );

                if (ext != ".obj" &&
                    ext != ".fbx" &&
                    ext != ".gltf" &&
                    ext != ".glb" &&
                    ext != ".dae" &&
                    ext != ".3ds" &&
                    ext != ".blend")
                {
                    continue;
                }

                if (ImGui::Selectable(name.c_str()))
                {
                    std::string selectedFile = entry.path().string();

                    std::cout << "Selected: " << selectedFile << '\n';

                    openFileBrowser = false;

                    std::string path = selectedFile;
                    std::string name = fs::path(selectedFile).stem().string();

                    engine->AddAMesh(path, name, { 0,5,0 }, { 10,10,10 }, false, true);
                }
            }
        }

        ImGui::End();
    }

    ImGui::End();

    CanChange = false;
}

void MakeGui::MakeIMViewPort(Window& wnd)
{
}

bool MakeGui::MakeDashBoard(IRenderer* renderer)
{
    bool openProject = false;
    static bool showConfigWindow = false;
    static bool showProjectsWindow = false;
    static bool showNewProjectWindow = false;

    MakeStyle();
    ImGui::SetNextWindowSize(ImVec2(screen_width, screen_height), ImGuiCond_Always);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f,0.0f,0.0f, 1.0f)); //Color

    float sh = screen_height;
    float sw = screen_width;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

    ImGui::Begin("Backround", nullptr,
        ImGuiWindowFlags_NoTitleBar | //No TitleBar
        ImGuiWindowFlags_NoResize |   //No resize
        ImGuiWindowFlags_NoMove |     //No Moving
        ImGuiWindowFlags_NoCollapse |  //No Smalling
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBringToFrontOnFocus //No Bring To Front
    );

    ImGui::PopStyleColor();

    ImVec2 windowSize = ImGui::GetWindowSize();
    float size = Min(windowSize.x, windowSize.y);

    ImVec2 buttonSize = ImVec2(size / 8, size / 8);
    ImVec2 NewProjectSize = ImVec2(sw / 8, sh / 16);

    ImGui::SetCursorPos(ImVec2(
        (windowSize.x + sw / 5 - NewProjectSize.x) * 0.5f,
        (windowSize.y - NewProjectSize.y + sh / 5) * 0.5f
    ));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 100.0f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.086f, 0.769f, 0.212f,1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.096f, 0.869f, 0.312f,1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.076f, 0.669f, 0.112f,1.0f));

    if (ImGui::Button("New Project", NewProjectSize)) {
        showNewProjectWindow = true;
        
        showProjectsWindow = false;
        showConfigWindow = false;
    }

    static Image2d image2d;
    static bool Load = false;
    if (!Load) {
        Load = true;
        std::string fullPath = textures + "\\TestTexture.png";

        image2d.LoadImGuiImage(renderer, fullPath);
    }
    ImGui::Begin("Test##Frame");

    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

    image2d.Draw(ImVec2(200, 200));

    ImGui::End();
    

    if (showNewProjectWindow) {
        ImGui::SetNextWindowSize(ImVec2(sw / 3, sh / 2), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(sw / 2, sh / 2), ImGuiCond_Once);

        ImGui::Begin("New Project ##Frame");

        static char projectname[30] = "";

        ImGui::Text("ProjectName: ");
        ImGui::SameLine();
        ImGui::InputTextWithHint(
            "##ProjectName",
            "e.g. MyAwesomeGame",
            projectname,
            sizeof(projectname)
        );

        static int value = 0;

        const char* templates[] = { "Void", "Baseplate", "Ducks" };

        ImGui::Text("Template (Not made yet): ");
        ImGui::SameLine();
        ImGui::Combo("##TemplateCombo", &value, templates, IM_ARRAYSIZE(templates));

        ImVec2 buttonSize(sw / 20, sh / 20);

        ImGui::SetCursorPos(
            ImVec2(
                ImGui::GetWindowSize().x - buttonSize.x - 10,
                ImGui::GetWindowSize().y - buttonSize.y - 10
            )
        );

        if (ImGui::Button("Create", buttonSize))
        {
            if (projectname[0] != '\0')
            {
                ProjectName = projectname;

                openProject = true;
            }
            else {
                CreateError("ProjectName cannot be empty!");
            }
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);

    ImGui::SetCursorPos(ImVec2(
        sw / 100,
        sh / 100
    ));

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(sw / 5, sh));
    ImGui::Begin("Discover ##1", nullptr,
        ImGuiWindowFlags_AlwaysVerticalScrollbar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDocking
    );

    if (ImGui::Button("Config Style")) {
        showConfigWindow = !showConfigWindow;

        if (showConfigWindow) {
            showProjectsWindow = false;
            showNewProjectWindow = false;
        } 
    }

    if (ImGui::Button("Projects")) {
        showProjectsWindow = !showProjectsWindow;

        if (showProjectsWindow) {
            showNewProjectWindow = false;
            showConfigWindow = false;
        }
    }

    if (showProjectsWindow)
    {
        showConfigWindow = false;

        float discoverWidth = sw / 5.0f;

        ImGui::SetNextWindowPos(ImVec2(discoverWidth, 0), ImGuiCond_Always);

        ImGui::SetNextWindowSize(
            ImVec2(sw - discoverWidth, sh),
            ImGuiCond_Always
        );

        ImGui::PushStyleColor(
            ImGuiCol_WindowBg,
            ImVec4(0.0f, 0.0f, 0.0f, 1.0f)
        );

        ImGui::Begin(
            "Projects##Page",
            nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDocking
        );

        float buttonSize = 100.0f;
        float padding = 10.0f;

        float windowVisibleX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

        for (const auto& entry : fs::directory_iterator(savings + "\\"))
        {
            if (!entry.is_directory())
                continue;

            std::string folderName =
                entry.path().filename().string();

            ImGui::PushID(folderName.c_str());

            if (ImGui::Button(folderName.c_str(),
                ImVec2(buttonSize, buttonSize)))
            {
                ProjectName = folderName;
                openProject = true;
            }

            ImGui::PopID();

            float lastButtonX =
                ImGui::GetItemRectMax().x;

            float nextButtonX =
                lastButtonX + padding + buttonSize;

            if (nextButtonX < windowVisibleX)
                ImGui::SameLine();
        }

        ImGui::PopStyleColor();
        ImGui::End();
    }
    ImGui::End();

    
    if (showConfigWindow) {
        showProjectsWindow = false;
        ImGui::SetNextWindowSize(ImVec2(sw / 2, sh), ImGuiCond_Always);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); //Color

        ImGui::SetNextWindowPos(ImVec2(sw / 2,0), ImGuiCond_Always);

        ImGui::Begin("Test Area", nullptr,
            ImGuiWindowFlags_NoResize |   //No resize
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDocking
        );

        ImGui::Button("Test Button", ImVec2(size / 10, size / 10));
        ImGui::PopStyleColor();
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(sw / 2, sh));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Style --Press Ctrl-Z to Go Back", nullptr,
            ImGuiWindowFlags_AlwaysVerticalScrollbar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse
        );

        bool ctrlPressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
        bool zPressed = ImGui::IsKeyPressed(ImGuiKey_Z);
        if (ctrlPressed && zPressed) {
            showConfigWindow = false;
        }

        if (ImGui::Button("Go Back")) {
            showConfigWindow = false;
        }
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

        style.CreateImGuiCustomization();
    }

    ImGui::End();

    return openProject;
}