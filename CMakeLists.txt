cmake_minimum_required(VERSION 3.10)
project(BoronEngine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

##How to add a new shader to DX11
##You just need add it's properties to SHADER_FILES, SHADER_NAMES, SHADER_TYPES,SHADER_FILES

if(MSVC)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug,RelWithDebInfo>,ProgramDatabase,Embedded>")
    
    add_compile_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/Zi>)
    
    add_compile_options($<$<CONFIG:Debug>:/Od>)
    
    add_compile_options($<$<CONFIG:Debug>:/RTC1>)
    
    add_link_options($<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:/DEBUG:FULL>)
endif()

find_package(Vulkan REQUIRED)

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan SDK not found.\nInstall it from https://vulkan.lunarg.com/")
endif()

message(STATUS "Using Vulkan SDK at: ${Vulkan_INCLUDE_DIRS}")

#GLFW
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_Declare(
    BoronMathLibrary
    GIT_REPOSITORY https://github.com/Kahviz/BoronMathLibrary
    GIT_TAG master
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(BoronMathLibrary)

#ImGui
set(IMGUI_SOURCES
    Core/Libs/ImGui/imgui.cpp
    Core/Libs/ImGui/imgui_draw.cpp
    Core/Libs/ImGui/imgui_tables.cpp
    Core/Libs/ImGui/imgui_widgets.cpp
    Core/Libs/ImGui/backends/imgui_impl_glfw.cpp
    Core/Libs/ImGui/backends/imgui_impl_dx11.cpp 
    Core/Libs/ImGui/backends/imgui_impl_vulkan.cpp
)

#Definitions
add_definitions(
    -DPROJECT_DIR="${CMAKE_SOURCE_DIR}/"
)

#Executable
add_executable(BoronEngine
    ${IMGUI_SOURCES}
    main.cpp #Entrypoint
    BoronGameEngine.h

    Core/SetupPaths/MakeFiles.cpp

    Core/Variables/GLOBALS.cpp
    Core/Services/Raycasting.cpp

    #Runtime
    Core/Runtime/Engine.cpp

    #Window
    Core/Window/Window.cpp

    #Debugging
    Core/Debugging/Profiler/Profiler.cpp
    Core/Debugging/Functions/BGE_ASSERTS.h
    Core/Multiplatform/BoronPlatform.h

    #Physics
    
    Core/BoronPhysics/Physics.h
    Core/BoronPhysics/Physics.cpp
    
    #Editor
    Editor/Gui/MakeGui.cpp
    Editor/Gui/Style.h
    Editor/Gui/Style.cpp

    #Camera
    Core/Camera/Camera.cpp
    Editor/Camera/CameraControl.cpp


    #VulkanRenderer Subclasses
    Core/Graphics/Vulkan/VulkanDevice/VulkanDevice.h
    Core/Graphics/Vulkan/VulkanDevice/VulkanDevice.cpp
    Core/Graphics/Vulkan/VulkanInstance/VulkanInstance.h
    Core/Graphics/Vulkan/VulkanInstance/VulkanInstance.cpp
    Core/Graphics/Vulkan/VulkanSwapchain/VulkanSwapchain.h
    Core/Graphics/Vulkan/VulkanSwapchain/VulkanSwapchain.cpp
    Core/Graphics/Vulkan/VulkanCommandBuffer/VulkanCommandBuffer.h
    Core/Graphics/Vulkan/VulkanCommandBuffer/VulkanCommandBuffer.cpp
    Core/Graphics/Vulkan/VulkanPipeline/VulkanPipeline.h
    Core/Graphics/Vulkan/VulkanPipeline/VulkanPipeline.cpp
    Core/Graphics/Vulkan/ScoreCounter.cpp

    #Adapters
    Core/Graphics/Dx11/Dx11Adapter.h
    Core/Graphics/Vulkan/VulkanAdapter.h
    Core/Graphics/Dx11/Dx11Adapter.cpp
    Core/Graphics/Vulkan/VulkanAdapter.cpp

    #Graphics
    Core/Graphics/Graphics.cpp
    Core/Graphics/GraphicsBackends.h

    #Texture
    Core/Graphics/Texture/Texture.h 
    Core/Graphics/Texture/Texture.cpp 

    #Mesh
    Core/Instances/Instances/Mesh/DX11/MeshDX11.cpp
    Core/Instances/Instances/Mesh/Vulkan/MeshVulkan.cpp

    #Renderers
    Core/Graphics/Dx11/Dx11Renderer.cpp
    Core/Graphics/Vulkan/VulkanRender.cpp

    #Saving
    Core/Misc/Saving/SaveProject.h 
    Core/Misc/Saving/SaveProject.cpp

    #Misc
    Core/Misc/Colission/Colission.cpp
    Core/Misc/Controller/Controller.cpp
    Core/Misc/Timer/Timer.cpp
    Core/Variables/Enums.h
 "Editor/Gui/Image2d.h" "Editor/Gui/Image2d.cpp")

target_include_directories(BoronEngine PRIVATE
    include
    Core
    Editor/
    ../Editor/
    Core/Libs/Assimp/include
    Core/Libs/SDL3
    Core/Libs/ImGui
    Core/Libs/ImGui/backends
    Core/Instances/Instances/
    Core/Instances/
    Core/Variables/
    Core/Debugging/Functions/

    Core/Graphics
    Core/Graphics/Texture/
    Core/Graphics/Dx11/
    Core/Graphics/Vulkan/

    Editor/Camera/
    Core/Services/
    Libs/STBIcons/
    ${Vulkan_INCLUDE_DIRS}
    ${boronmathlibrary_SOURCE_DIR}
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


# Copy HLSL Files to build-kansioon
foreach(shader_file ${SHADER_FILES})
    set(source_file "${CMAKE_SOURCE_DIR}/Core/Shaders/${shader_file}")
    
    add_custom_command(TARGET BoronEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${source_file}
        $<TARGET_FILE_DIR:BoronEngine>
        COMMENT "Copying ${shader_file}"
    )
endforeach()

if(WIN32)
    list(LENGTH SHADER_FILES SHADER_COUNT)
    
    foreach(index RANGE ${SHADER_COUNT})
        math(EXPR current_index "${index} - 1")
        
        if(current_index GREATER -1 AND current_index LESS ${SHADER_COUNT})
            list(GET SHADER_FILES ${current_index} shader_file)
            list(GET SHADER_TYPES ${current_index} shader_type)
            
            string(REPLACE ".hlsl" "" shader_name ${shader_file})
            
            set(output_file "${CMAKE_CURRENT_BINARY_DIR}/${shader_name}.cso")
            set(input_file "${CMAKE_SOURCE_DIR}/Core/Shaders/${shader_file}")
            
            add_custom_command(
                OUTPUT ${output_file}
                COMMAND ${CMAKE_COMMAND} -E remove -f ${output_file}
                COMMAND fxc.exe /T ${shader_type} /E main /Fo "${output_file}" "${input_file}"
                DEPENDS ${input_file}
                COMMENT "Compiling ${shader_file} -> ${shader_name}.cso"
            )
            
            list(APPEND GENERATED_SHADERS ${output_file})
            
            add_custom_command(TARGET BoronEngine POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${output_file}
                $<TARGET_FILE_DIR:BoronEngine>
                COMMENT "Copying ${shader_name}.cso"
            )
        endif()
    endforeach()
    
    if(GENERATED_SHADERS)
        add_custom_target(CompileShaders DEPENDS ${GENERATED_SHADERS})
        add_dependencies(BoronEngine CompileShaders)
    endif()
endif()

if(MSVC)
    set(CMAKE_VS_NO_COMPILE_BATCHING ON)
endif()