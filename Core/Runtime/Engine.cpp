#include "Engine.h"
#include <stdexcept>
#include "chrono"
#include <iostream>
#include <Instance.h>
#include <Object.h>
#include <GLOBALS.h>
#include <CameraControl.h>
#include "MakeGui.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_vulkan.h"

Engine::Engine()
    : window(1280, 800, "UntilitedGameEngine")
{
    if (!ImGuiInited) {
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
        #else
            VulkanRender* vr = window.GetGraphics().VR.get();

            ImGui_ImplVulkan_InitInfo init_info = {};
            init_info.Instance = vr->GetVulkanInstance();
            init_info.PhysicalDevice = vr->physicalDevice;
            init_info.Device = vr->device;
            init_info.QueueFamily = vr->GetGraphicsFamilyIndex();
            init_info.Queue = vr->graphicsQueue;
            init_info.PipelineCache = VK_NULL_HANDLE;
            init_info.DescriptorPool = vr->GetImGuiPool();
            init_info.MinImageCount = 2;
            init_info.ImageCount = vr->GetSwapChainImageViews().size();

            init_info.Allocator = nullptr;
            init_info.CheckVkResultFn = [](VkResult err) {
                if (err != VK_SUCCESS) {
                    std::cerr << "Vulkan Error: " << err << std::endl;
                }
            };
            VkRenderPass renderPass = vr->GetRenderPass();

            if (!ImGui_ImplVulkan_Init(&init_info)) {
                throw std::runtime_error("Failed to initialize ImGui Vulkan backend");
            }
            else {
                ImGuiInited = true;
            }
        #endif

        #ifdef _DEBUG
            std::cout << "ImGui initialized successfully!" << std::endl;
        #endif

        window.SetWindowIcon(window.GetWindow());
        ImGuiIO& IO = ImGui::GetIO();
        IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
}

Engine::~Engine()
{
    if (ImGuiInited) {
        #if DIRECTX11 == 1
            ImGui_ImplDX11_Shutdown();
        #endif

        #if VULKAN == 1
            ImGui_ImplVulkan_Shutdown();
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
    auto lastFrameTime = clock::now();
    ImGuiIO& IO = ImGui::GetIO();

    while (!glfwWindowShouldClose(glfwWND))
    {
        auto now = clock::now();
        std::chrono::duration<float> delta = now - lastFrameTime;
        float deltaTime = delta.count();
        lastFrameTime = now;

        EngineDoFrame(&window, deltaTime);

        glfwPollEvents();
    }

    return 0;
}

Instance& Engine::AddAMesh(const std::string& Path, const std::string& Name,
    FLOAT3 pos, FLOAT3 Size, bool Selec)
{
    auto obj = std::make_unique<Object>(
        Name,
        1,
        pos,
        Size,
        INT3(
            static_cast<int>(Color3.x * 255.0f),
            static_cast<int>(Color3.y * 255.0f),
            static_cast<int>(Color3.z * 255.0f)
        )
    );

    obj->UniqueID = Index;

#if DIRECTX11 == 1
    obj->OBJmesh.DM.Load(assets + Path, window.GetGraphics().GetDevice());

    #ifdef _DEBUG
        std::cout << "DX11 Mesh loaded: " << assets + Path << std::endl;
        std::cout << "Vertices: " << obj->OBJmesh.GetVertices().size() << std::endl;
        std::cout << "Indices: " << obj->OBJmesh.GetIndices().size() << std::endl;
    #endif
#endif

#if VULKAN == 1
    obj.get()->OBJmesh.VM.Load(
        assets + Path,
        window.GetGraphics().GetDevice(),
        window.GetGraphics().GetPhysicalDevice(),
        window.GetGraphics().VR.get()->commandPool,
        window.GetGraphics().VR.get()->graphicsQueue
    );
#endif

#ifdef _DEBUG
    std::cout << "Loading mesh: " << assets + Path << std::endl;

    #if VULKAN == 1
        std::cout << "Vertices: " << obj.get()->OBJmesh.VM.GetVertices().size() << ", Indices: " << obj.get()->OBJmesh.VM.GetIndices().size() << std::endl;
    #endif
#endif
    obj->Selected = Selec;

    Instance* objPtr = obj.get();
    Drawables.push_back(std::move(obj));

    Index++;
    return *objPtr;
}

void ScreenResizerDetector(Window* wnd) {
    int width, height;
    glfwGetFramebufferSize(wnd->GetWindow(), &width, &height);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.DisplaySize = ImVec2((float)width, (float)height);

    int window_width, window_height;
    glfwGetWindowSize(wnd->GetWindow(), &window_width, &window_height);

    io.DisplayFramebufferScale = ImVec2(
        window_width > 0 ? (float)width / (float)window_width : 1.0f,
        window_height > 0 ? (float)height / (float)window_height : 1.0f
    );

    if (screen_width != width || screen_height != height) {
        wnd->GetGraphics().ReSizeWindow(width, height, wnd);
        screen_width = width;
        screen_height = height;

        #ifdef _DEBUG
            std::cout << "Screen resized to: " << screen_width << "x" << screen_height << std::endl;
        #endif
    }
}

void Engine::EngineDoFrame(Window* wnd, float deltatime)
{
    if (ImGui::GetCurrentContext() == nullptr) {
        std::cerr << "ERROR: No ImGui context set!" << std::endl;
        return;
    }

    Graphics& graphics = wnd->GetGraphics();

    #if VULKAN == 1
       VulkanRender* vr = graphics.VR.get();
    #endif
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
    #else
        graphics.SetRenderTargetToBackBuffer();
    #endif

    graphics.ClearBuffer(0.0f, 0.0f, 1.0f);

    static bool CubeB = false;

    if (!CubeB) {
        window.GetGraphics().ReSizeWindow(screen_width, screen_height, wnd);
        AddAMesh("\\Cube.obj", "TestCube", { 0,0,0 }, { 0.5,0.5,0.5 }, false);

        CubeB = true;
    }

    bool ctrlPressed = (glfwGetKey(wnd->GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
    if (ctrlPressed) {
        AddAMesh("\\Cylinder.obj", "TestCylinder", { 1,0,0 }, { 0.5,1,0.5 }, false);
    }

#if INEDITOR == 1
    graphics.SetRenderTargetToScene();

    graphics.ClearSceneBuffer(0.1f, 0.2f, 0.3f);
#endif

    graphics.DrawAFrame(deltatime, Drawables);

#if INEDITOR == 1
    graphics.SetRenderTargetToBackBuffer();
#endif

#if INEDITOR == 1
    if (ImGuiInited) {
        makeGui.MakeIMViewPort(*wnd);

        makeGui.MakeIMGui(
            *wnd,
            Drawables,
            [this](const std::string& path, const std::string& name,
                FLOAT3 pos, FLOAT3 size, bool Selec) -> Instance*
            {
                return &AddAMesh(path, name, pos, size, Selec);
            },
            reinterpret_cast<float*>(&Color3),
            false
        );
    }
#endif
    //Fps
    int fps = static_cast<int>(1.0f / deltatime);
    MakeASuccess("FPS: " + std::to_string(fps));

    //Draws
    for (auto& Drawableptr : Drawables) {
        auto Drawable = Drawableptr.get();
        if (Drawable->CanDraw()) {
            #if VULKAN == 1
                 wnd->GetGraphics().VR.get()->RenderAMesh(Drawable, Drawable->Orientation, Drawable->pos, Drawable->Size, Drawable->color, Drawable->Velocity, Drawable->Anchored, 1.0f, 1.0f, Drawable->UniqueID);
            #endif
        }
    }

    CameraControl camC;
    if (!ctrlPressed)
        camC.MakeCameraControls(*wnd, deltatime);

#if INEDITOR == 1 
    if (ImGuiInited) {
        ImGui::Render();
        #if DIRECTX11 == 1
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        #endif
        #if VULKAN == 1
            //ImGui_ImplDX11_RenderDrawData tänne
        #endif
    }
#endif

    wnd->GetGraphics().EndFrame();
}