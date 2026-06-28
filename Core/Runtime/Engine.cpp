#include "Engine.h"
#include <stdexcept>
#include "chrono"
#include <iostream>
#include <Object.h>
#include <GLOBALS.h>
#include "../Editor/Camera/CameraControl.h"
#include "BGE_ASSERTS.h"

#ifdef _WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3native.h>

#include "backends/imgui_impl_glfw.h"
#include "GraphicsBackends.h"

#include <random>

Engine::Engine()
    : window(1280, 800, "BoronEngine")
{
    if (!ImGuiInited) {
        std::cout << "ImGui version: " << IMGUI_VERSION << std::endl;
        std::cout << "BoronMathVersion: " << BORONMATHversion << std::endl;
        IMGUI_CHECKVERSION();
        ImGuiContext* context = ImGui::CreateContext();

        ImGui::SetCurrentContext(context);

        ImGui::StyleColorsDark();

        GLFWwindow* glfwWindow = window.GetWindow();
#if DIRECTX11 == 1
        ID3D11Device* device = window.GetGraphics().GetDevice();
        ID3D11DeviceContext* contextDX11 = window.GetGraphics().GetpContext();
#endif

        if (!ImGui_ImplGlfw_InitForOther(glfwWindow, true)) {
            throw std::runtime_error("Failed to initialize ImGui GLFW backend");
        }

#if DIRECTX11 == 1 
        if (!ImGui_ImplDX11_Init(device, contextDX11)) {
            throw std::runtime_error("Failed to initialize ImGui DX11 backend");
        }
        else {
            ImGuiInited = true;
        }
#endif
#if VULKAN == 1
        auto& vk = static_cast<VulkanAdapter&>(window.GetGraphics().GetRenderer());

        VkRenderPass renderPass = vk.GetRenderPass();
        if (renderPass == VK_NULL_HANDLE) {
            throw std::runtime_error("Render pass is null!");
        }

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.ApiVersion = VK_API_VERSION_1_2;
        init_info.Instance = vk.GetInstance();
        init_info.PhysicalDevice = vk.GetPhysicalDevice();
        init_info.Device = vk.GetDevice();
        init_info.QueueFamily = vk.GetGraphicsFamilyIndex();
        init_info.Queue = vk.GetGraphicsQueue();
        init_info.DescriptorPool = vk.GetImGuiPool();
        init_info.MinImageCount = 2;
        init_info.ImageCount = static_cast<uint32_t>(vk.GetSwapChainImageViews().size());
        init_info.PipelineCache = VK_NULL_HANDLE;

        init_info.PipelineInfoMain.RenderPass = renderPass;
        init_info.PipelineInfoMain.Subpass = 0;

        init_info.PipelineInfoForViewports = init_info.PipelineInfoMain;
        init_info.UseDynamicRendering = false;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = [](VkResult err) {
            if (err != VK_SUCCESS) {
                std::cerr << "Vulkan Error: " << err << std::endl;
            }
        };

        if (!ImGui_ImplVulkan_Init(&init_info)) {
            throw std::runtime_error("Failed to initialize ImGui Vulkan backend");
        }

        ImGuiInited = true;
#endif
        ImGuiIO& io = ImGui::GetIO();
        std::string fontPath = fonts + "\\RobotoFont.ttf";
        ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f);

        if (font == nullptr) {
            CreateError("Could not load font, using default font");
            io.Fonts->AddFontDefault();
        }

        #ifdef _DEBUG
        CreateSuccess("ImGui initialized successfully!");
        #endif

        window.SetWindowIcon(window.GetWindow());
        ImGuiIO& IO = ImGui::GetIO();
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        IO.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
    }
}
Engine::~Engine()
{
    if (ImGuiInited) {
        #if DIRECTX11 == 1
            ImGui_ImplDX11_Shutdown();
        #endif //Vulkan Does it in CleanUp

        #if VULKAN == 1
            auto& vk = static_cast<VulkanAdapter&>(window.GetGraphics().GetRenderer());

            for (auto& Drawable : Drawables) {
                if (Drawable->GetTexture()->IsLoaded()) {
                    Drawable->GetTexture()->Cleanup(vk.GetDevice());
                }
            }

            window.GetGraphics().GetRenderer().CleanUp();
        #endif
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImGuiInited = false;
    }
}

int Engine::EngineRun()
{
    GLFWwindow* glfwWND = window.GetWindow();

    using clock = std::chrono::high_resolution_clock;
    ImGuiIO& IO = ImGui::GetIO();

    #if INEDITOR == 0
        InProject = true;
    #endif
    
    auto lastFrameTime = clock::now();

    while (!glfwWindowShouldClose(glfwWND))
    {
        auto now = clock::now();

        float deltaTime =
            std::chrono::duration<float>(now - lastFrameTime).count();

        lastFrameTime = now;

        EngineDoFrame(&window, deltaTime);

        glfwPollEvents();
    }

    SaveProject::Save(Drawables);

    profiler.PrintInformation();

    return 0;
}

Instance* Engine::AddAMesh(const std::string& Path, const std::string& Name,
    BML::Vector3 pos, BML::Vector3 Size, bool Selec,bool LiteralPath)
{
    Transform transform;
    transform.Position = pos;
    transform.Size = Size;

    auto obj = std::make_unique<Object>(
        Name,
        1,
        BML::Int3(
            static_cast<int>(Color3.x() * 255.0f),
            static_cast<int>(Color3.y() * 255.0f),
            static_cast<int>(Color3.z() * 255.0f)
        ),
        BML::Int3(168, 160, 160),
        BML::Vector3(0, 0, 0),
        transform,
        true,
        std::make_shared<Mesh>()
    );

    obj->UniqueID = Index;

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
        obj.get()->OBJmesh = Mesh::Load(
            assets + Path,
            vk.GetDevice(),
            vk.GetPhysicalDevice(),
            vk.GetCommandPool(),
            vk.GetGraphicsQueue()
        );
    }
    else {
        obj.get()->OBJmesh = Mesh::Load(
            Path,
            vk.GetDevice(),
            vk.GetPhysicalDevice(),
            vk.GetCommandPool(),
            vk.GetGraphicsQueue()
        );
    }
    
#endif

    obj->Selected = Selec;

    std::string fullPath = textures + "\\TestTexture.png";

#if DIRECTX11 == 1
    obj->texture.Load(fullPath, window.GetGraphics().GetRenderer());
#endif
#if VULKAN == 1
    obj->texture.LoadVK(fullPath, vk);
    vk.UpdateDescriptorSet(obj.get()); //Updates DescriptorSets so the texture is loaded in the renderer
#endif
    obj->Parent = &world;

    Instance* objPtr = obj.get();
    Drawables.push_back(std::move(obj));

    Index++;
    return objPtr;
}

void ScreenResizerDetector(Window* wnd) {
    static int lastWidth = 0, lastHeight = 0;
    glfwGetFramebufferSize(wnd->GetWindow(), &screen_width, &screen_height);

    if (screen_width != lastWidth || screen_height != lastHeight) {
        wnd->GetGraphics().ReSizeWindow(screen_width, screen_height, wnd);
        lastWidth = screen_width;
        lastHeight = screen_height;
        #ifdef _DEBUG
            std::cout << "Screen resized to: " << screen_width << "x" << screen_height << std::endl;
        #endif
    }

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)screen_width, (float)screen_height);

    int window_width, window_height;
    glfwGetWindowSize(wnd->GetWindow(), &window_width, &window_height);
    io.DisplayFramebufferScale = ImVec2(
        window_width > 0 ? (float)screen_width / (float)window_width : 1.0f,
        window_height > 0 ? (float)screen_height / (float)window_height : 1.0f
    );
}



float GetRandomFloat(float min, float max) { //Mathlib
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

void Engine::EngineDoFrame(Window* wnd, float deltatime)
{
    static float timer = 0.0f;
    static int framesd = 0;

    timer += deltatime;
    framesd++;

    if (timer >= 1.0f)
    {
        std::cout << "FPS: " << framesd << '\n';
        timer = 0.0f;
        framesd = 0;
    }

    bool ctrlPressed = (glfwGetKey(wnd->GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
    bool RctrlPressed = (glfwGetKey(wnd->GetWindow(), GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);

    static int frames = 0;
    static int cubes = 0;
    frames++;

    if (frames == 1000) {
        frames = 0;
    }

    if (ctrlPressed) {
        
        AddAMesh("\\Cube.obj", "Cube", { GetRandomFloat(-50,50),GetRandomFloat(-50,50),GetRandomFloat(-50,50) }, {1,1,1}, false,false);

        cubes++;
    }
    if (RctrlPressed) {
        AddAMesh("\\Cylinder.obj", "Cylinder", { GetRandomFloat(-50,50),GetRandomFloat(-50,50),GetRandomFloat(-50,50) }, { 1,1,1 }, false,false);

        cubes++;
        std::cout << "FPS: " << (1.0f / deltatime) << '\n';
        std::cout << "Cubes: " << (cubes) << '\n';
    }


    if (ImGui::GetCurrentContext() == nullptr) {
        std::cerr << "ERROR: No ImGui context set!" << std::endl;
        return;
    }

    static bool Init = false;

    Graphics& graphics = wnd->GetGraphics();

    ScreenResizerDetector(wnd);

#if INEDITOR == 1
    if (ImGuiInited) {
        #if DIRECTX11 == 1
            ImGui_ImplDX11_NewFrame();
        #endif

        #if VULKAN == 1
            ImGui_ImplVulkan_NewFrame();
        #endif
        
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
#endif

    graphics.ClearBuffer(0.0f, 0.0f, 1.0f);

    if (InProject) {
        if (!Init) {
            CreateSuccess("Initing drawables");

            world.Name = "World";
            world.Parent = nullptr;
            world.InstanceType = Boron::Enums::InstanceType::World;
            world.UniqueID = -1;
            Drawables = SaveProject::Load(window,world);

            for (auto& Drawable : Drawables) {
                if (Drawable->Parent == nullptr) {
                    world.AddChild(Drawable.get());
                }
            }
            if (Drawables.empty()) { 
                SaveProject::Save(Drawables);

                Instance* inst = AddAMesh("\\Cube.obj", "Cube2", BML::Vec3{ 0,0,0 }, BML::Vec3{ 0.5,1,0.5 }, false,false);
                Instance* inst2 = AddAMesh("\\Cube.obj", "Cube", BML::Vec3{ 0,-5,0 }, BML::Vec3{ 10,1,10 }, false, false);
                inst->AddChild(inst2);
            }
            
            wnd->GetGraphics().GetCamera().SetPosition(5, 5, 5);
            wnd->GetGraphics().GetCamera().SetRotation(-0.615f, -2.356f, 0.0f);
            Init = true;
        }
    }
#if INEDITOR == 1
    if (InProject && ImGuiInited) {
        makeGui.MakeIMViewPort(*wnd);
        makeGui.MakeIMGui(
            *wnd,
            Drawables,
            reinterpret_cast<float*>(&Color3),
            false,
            this,
            world,
            &graphics.GetRenderer()
        );
        
    }
    else {
        if (makeGui.MakeDashBoard(&graphics.GetRenderer())) {
            CreateInfo("Opened a project");
            InProject = true;
        }
    }

    makeGui.RenderPopUps(deltatime); //Here last so thay be rendered always and on top of everything
#endif

#if VULKAN == 1
    for (auto& Drawableptr : Drawables) {
        Instance* Drawable = Drawableptr.get();
        if (Drawable->CanDraw()) {
            wnd->GetGraphics().RenderAMesh(
                deltatime,
                Drawable
            );
        }
    }
#endif

    //Physics
    for (auto& Drawableptr : Drawables) {
        auto Drawable = Drawableptr.get();

        if (Drawable->Anchored) { continue; }
        if (!Drawable->CanDraw()) { continue; }

        physics.ApplyGravity(*Drawable,deltatime);
    }
#if DIRECTX11 == 1
    wnd->GetGraphics().DrawAFrame(deltatime, Drawables);
#endif    
    if (!ctrlPressed && !Typing) {
        camC.MakeCameraControls(*wnd, deltatime);
    }

#if INEDITOR == 1 
    if (ImGuiInited) {
        ImGui::Render();
        #if DIRECTX11 == 1
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        #endif
    }
#endif
    float fps = 1.0f / deltatime;
    profiler.AddFPS(fps);

#if VULKAN == 1
    wnd->GetGraphics().DrawAFrame(deltatime, Drawables);
#endif

    wnd->GetGraphics().EndFrame();
}