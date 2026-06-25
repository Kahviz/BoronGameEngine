#include "MakeGui.h"
#include <algorithm>
#include "imgui.h"
#include "GraphicsBackends.h"
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

void MakeChildrenNodes(Instance* inst,
    std::vector<std::unique_ptr<Instance>>& Drawables,
    Instance& world)
{
    if (!inst) return;

    std::vector<Instance*> children = inst->GetChildren();
    for (auto& child : children) {
        if (!child) continue;

        std::vector<Instance*> grandChildren = child->GetChildren();

        if (grandChildren.empty())
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf |
                ImGuiTreeNodeFlags_NoTreePushOnOpen;

            if (child->Selected) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool open = ImGui::TreeNodeEx((void*)child, flags, "%s", child->Name.c_str());

            if (ImGui::IsItemClicked()) {
                for (auto& i : Drawables) {
                    i->Deselect();
                }

                child->Select();
                CreateError("Selected child");
                world.Selected = false;
            }
        }
        else
        {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

            if (child->Selected) flags |= ImGuiTreeNodeFlags_Selected;

            bool open = ImGui::TreeNodeEx((void*)child, flags, "%s", child->Name.c_str());

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                for (auto& i : Drawables)
                    i->Deselect();

                child->Select();
                world.Selected = false;
            }
            if (open) {
                MakeChildrenNodes(child, Drawables, world);
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

void MakeFloat3Edit(const Instance& inst,std::string Name, BML::Vector3& vec) {
    char label[128];

    snprintf(label, sizeof(label), "%s:", (Name).c_str());

    ImGui::Text("%s", label);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat(("##x" + Name + std::to_string(inst.UniqueID)).c_str(), &vec.x(), 0.0f, 0.0f, "%g", ImGuiInputTextFlags_CharsDecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat(("##y" + Name + std::to_string(inst.UniqueID)).c_str(), &vec.y(), 0.0f, 0.0f, "%g", ImGuiInputTextFlags_CharsDecimal);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputFloat(("##z" + Name + std::to_string(inst.UniqueID)).c_str(), &vec.z(), 0.0f, 0.0f, "%g", ImGuiInputTextFlags_CharsDecimal);
}

void MakeGui::MakeIMGui(Window& wnd,
    std::vector<std::unique_ptr<Instance>>& Drawables,
    float* Color3,
    bool Selec,
    Engine* engine,
    Instance& world,
    IRenderer* renderer
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
    
    bool anyDrawableSelected = false;

    for (const auto& Drawable : Drawables) {
        if (Drawable.get()->Selected) {
            world.Selected = false;
            anyDrawableSelected = true;

            Instance& inst = *Drawable.get();

            // Name
            ImGui::Text("Name: ");
            ImGui::SameLine();
            InputTextStd("##Name", inst.Name);

            // Pos
            if (ImGui::TreeNode("Transform")) {
                MakeFloat3Edit(inst,"Position", inst.transform.Position);
                MakeFloat3Edit(inst,"Orientation", inst.transform.Orientation);
                MakeFloat3Edit(inst,"Size", inst.transform.Size);

                ImGui::TreePop();
            }

            // Anchored
            if (ImGui::TreeNode("Physics")) {
                ImGui::Checkbox("Anchored: ", &inst.Anchored);

                ImGui::TreePop();
            }
        }
    }

    if (!anyDrawableSelected && world.Selected) {
        ImGui::Text("Name: ");
        ImGui::SameLine();
        InputTextStd("##NameWORLD", world.Name);
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

    Image2d plusbutton;
    plusbutton.LoadImGuiImage(renderer, textures + "\\PlusIcon.png");
    static bool plusGuiOpen = false;
    static Instance* selectedInst = nullptr;
    if (worldNodeOpen)
    {
        for (auto& instPtr : Drawables)
        {
            Instance* inst = instPtr.get();

            if (!inst || inst->Parent != &world)
                continue;

            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_OpenOnArrow;

            if (inst->IsVisibleInExplorer) {
                bool nodeOpen = ImGui::TreeNodeEx((void*)inst, flags, "%s", inst->Name.c_str());
                bool clicked = ImGui::IsItemClicked();

                if (nodeOpen)
                {
                    MakeChildrenNodes(inst, Drawables, world);
                    ImGui::TreePop();
                }

                if (clicked)
                {
                    for (auto& i : Drawables) i->Deselect();
                    inst->Select();
                    world.Selected = false;
                }

                if (inst->Selected) {
                    float aspect = screen_w / screen_h;
                    float size = aspect * 8;
                    float padding = ImGui::GetStyle().WindowPadding.x;
                    float scrollbarSize = ImGui::GetStyle().ScrollbarSize;

                    ImVec2 savedCursor = ImGui::GetCursorPos();
                    float itemTopY = ImGui::GetItemRectMin().y - ImGui::GetWindowPos().y + ImGui::GetScrollY();

                    ImGui::SetCursorPos(ImVec2(
                        ImGui::GetWindowWidth() - size - scrollbarSize - padding,
                        itemTopY
                    ));

                    std::string id = "##plusbutton" + std::to_string(inst->UniqueID);

                    if (ImGui::ImageButton(id.c_str(), plusbutton.GetTexture(), ImVec2(size, size))) {
                        plusGuiOpen = true;
                        selectedInst = inst;
                    }

                    ImGui::SetCursorPos(savedCursor);
                    ImGui::Dummy(ImVec2(0.0f, 0.0f));
                }
            }
        }

        ImGui::TreePop();
    }
    ImGui::End();
    if (plusGuiOpen && selectedInst) {
        ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_Once);
        ImGui::Begin("Add a instance", &plusGuiOpen);

        if (ImGui::Selectable("Object")) {
            Instance* inst = engine->AddAMesh(
                "\\Cube.obj", "Object", { 0,5,0 }, { 2,2,2 }, false, false
            );
            if (selectedInst && inst) {
                selectedInst->Children.push_back(inst);
                inst->Parent = selectedInst;
            }
            plusGuiOpen = false;
        }

        ImGui::End();
    }

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

void MakeGui::CreateErrorPopUp(IRenderer* renderer, Image2d& image2d, const std::string& errormsg, const float duration) {
    std::string fullPath = textures + "\\ErrorIcon.png";

    image2d.LoadImGuiImage(renderer, fullPath);

    Image image;
    image.image2d = image2d;
    image.duration = duration;
    image.reason = errormsg;
    images.push_back(image);
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
                Image2d errorPopUp;
                CreateErrorPopUp(renderer, errorPopUp, "ProjectName cannot be empty!", 2.0f);
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

void MakeGui::RenderPopUps(float deltatime)
{
    int renderIndex = 0;

    for (auto it = images.begin(); it != images.end();)
    {
        std::string windowName = "##Popup" + std::to_string(renderIndex);

        float popupWidth = screen_width / 4.0f;
        float popupHeight = screen_height / 5.0f;

        ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight));

        float overlapOffset = 15.0f;

        ImGui::SetNextWindowPos(
            ImVec2(
                screen_width * 0.5f,
                50.0f + renderIndex * overlapOffset
            ),
            ImGuiCond_Always,
            ImVec2(0.5f, 0.0f)
        );

        ImGui::Begin(
            windowName.c_str(),
            nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoMove
        );

        ImVec2 windowSize = ImGui::GetWindowSize();

        float aspect = screen_width / screen_height;
        float size = aspect * 100.0f;

        ImGui::SetCursorPos(ImVec2(
            screen_width / 30,
            (windowSize.y - size) * 0.5f
        ));

        it->image2d.Draw(ImVec2(size, size));

        ImGui::SameLine();

        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", it->reason.c_str());
        ImGui::End();

        it->duration -= deltatime;

        if (it->duration <= 0.0f)
            it = images.erase(it);
        else
            ++it;

        renderIndex++;
    }
}