cmake_minimum_required(VERSION 3.10)
project(BoronEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<CONFIG:Debug,RelWithDebInfo>,ProgramDatabase,Embedded>")

    add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Zi>)
    add_compile_options($<$<CONFIG:Debug>:/Od>)
    add_compile_options($<$<CONFIG:Debug>:/RTC1>)
    add_link_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG:FULL>)
endif()

find_package(Vulkan REQUIRED)

message(STATUS "Using Vulkan SDK at: ${Vulkan_INCLUDE_DIRS}")

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_Declare(
    EclipseMathLibrary
    GIT_REPOSITORY https://github.com/Kahviz/BoronMathLibrary
    GIT_TAG master
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(EclipseMathLibrary)

set(IMGUI_SOURCES
    Core/Libs/ImGui/imgui.cpp
    Core/Libs/ImGui/imgui_draw.cpp
    Core/Libs/ImGui/imgui_tables.cpp
    Core/Libs/ImGui/imgui_widgets.cpp
    Core/Libs/ImGui/backends/imgui_impl_glfw.cpp
    Core/Libs/ImGui/backends/imgui_impl_dx11.cpp
    Core/Libs/ImGui/backends/imgui_impl_vulkan.cpp
)

add_definitions(-DPROJECT_DIR="${CMAKE_SOURCE_DIR}/")

add_executable(BoronEngine
    ${IMGUI_SOURCES}
    main.cpp
    Core/SetupPaths/MakeFiles.cpp
    Core/Camera/Camera.cpp
    Core/ErrorHandling/Expectation/Expection.cpp
    Core/Graphics/Graphics.cpp
    Core/Instances/Instances/Mesh/DX11/MeshDX11.cpp
    Core/Instances/Instances/Mesh/Vulkan/MeshVulkan.cpp
    Core/Gui/ImGuiManager.cpp
    Core/Misc/Colission/Colission.cpp
    Core/Misc/Controller/Controller.cpp
    Core/Misc/Timer/Timer.cpp
    Core/Variables/GLOBALS.cpp
    Core/Services/Raycasting.cpp
    Core/Runtime/Engine.cpp
    Core/Window/Window.cpp
    Core/Graphics/Dx11/Dx11Renderer.cpp
    Core/Graphics/Vulkan/VulkanRender.cpp
    Core/Graphics/Vulkan/ScoreCounter.cpp
    Core/Debugging/Profiler/Profiler.cpp
    Core/Graphics/Texture/Texture.cpp
    Core/BoronPhysics/Physics.cpp
    Editor/Gui/MakeGui.cpp
    Editor/Camera/CameraControl.cpp

    Core/Graphics/Vulkan/VulkanDevice/VulkanDevice.cpp
    Core/Graphics/Vulkan/VulkanInstance/VulkanInstance.cpp
    Core/Graphics/Vulkan/VulkanSwapchain/VulkanSwapchain.cpp
    Core/Graphics/Vulkan/VulkanCommandBuffer/VulkanCommandBuffer.cpp
    Core/Graphics/Vulkan/VulkanPipeline/VulkanPipeline.cpp
)

target_include_directories(BoronEngine PRIVATE
    Core
    Editor
    Core/Libs/ImGui
    Core/Libs/ImGui/backends
    Core/Libs/Assimp/include
    Core/Libs/SDL3
    Core/Graphics/Texture
    Core/Services
    ${Vulkan_INCLUDE_DIRS}
    ${eclipsemathlibrary_SOURCE_DIR}
)

target_link_libraries(BoronEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/Libs/assimp-vc143-mt.lib
    ${CMAKE_SOURCE_DIR}/Libs/SDL3.lib
    Vulkan::Vulkan
    glfw
)

if(WIN32)
    target_link_options(BoronEngine PRIVATE "/SUBSYSTEM:CONSOLE")
endif()

if(WIN32)
    set(DLLS
        ${CMAKE_SOURCE_DIR}/Libs/assimp-vc143-mt.dll
        ${CMAKE_SOURCE_DIR}/Libs/SDL3.dll
    )

    foreach(dll ${DLLS})
        add_custom_command(TARGET BoronEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dll}
            $<TARGET_FILE_DIR:BoronEngine>
        )
    endforeach()
endif()

#Shaders
set(SHADER_FILES
    VertexShader.hlsl
    PixelShader.hlsl
    PixelShaderTexture.hlsl
)

set(SHADER_TYPES
    vs_5_0
    ps_5_0
    ps_5_0
)

# DX11 SHADERS
if(WIN32)
    list(LENGTH SHADER_FILES SHADER_COUNT)
    math(EXPR SHADER_MAX "${SHADER_COUNT} - 1")

    foreach(index RANGE 0 ${SHADER_MAX})
        list(GET SHADER_FILES ${index} shader_file)
        list(GET SHADER_TYPES ${index} shader_type)

        string(REPLACE ".hlsl" "" shader_name ${shader_file})

        set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${shader_name}.cso")
        set(input_file "${CMAKE_SOURCE_DIR}/Core/Shaders/${shader_file}")

        add_custom_command(
            OUTPUT ${output_file}
            COMMAND fxc.exe /T ${shader_type} /E main /Fo ${output_file} ${input_file}
            DEPENDS ${input_file}
        )

        list(APPEND GENERATED_SHADERS ${output_file})

        add_custom_command(TARGET BoronEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${output_file}
            $<TARGET_FILE_DIR:BoronEngine>
        )
    endforeach()

    add_custom_target(CompileShaders DEPENDS ${GENERATED_SHADERS})
    add_dependencies(BoronEngine CompileShaders)
endif()

if(MSVC)
    set(CMAKE_VS_NO_COMPILE_BATCHING ON)
endif()