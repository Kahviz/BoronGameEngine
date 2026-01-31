@echo off
echo === Compiling Vulkan Shaders ===

REM Vertex shader
if exist "Shaders\Source\vertex.glsl" (
    echo Compiling vertex.glsl...
    "C:\VulkanSDK\1.4.335.0\Bin\glslangValidator.exe" -V "Shaders\Source\vertex.glsl" -o "Shaders\vertex.spv" --target-env vulkan1.2 -S vert
    if errorlevel 1 (
        echo ERROR: Failed to compile vertex shader
        pause
        exit /b 1
    ) else (
        echo ✓ vertex.spv created
    )
) else (
    echo ERROR: vertex.glsl not found
    dir Shaders\Source\
    pause
    exit /b 1
)

REM Fragment shader
if exist "Shaders\Source\fragment.glsl" (
    echo Compiling fragment.glsl...
    "C:\VulkanSDK\1.4.335.0\Bin\glslangValidator.exe" -V "Shaders\Source\fragment.glsl" -o "Shaders\fragment.spv" --target-env vulkan1.2 -S frag
    if errorlevel 1 (
        echo ERROR: Failed to compile fragment shader
        pause
        exit /b 1
    ) else (
        echo ✓ fragment.spv created
    )
) else (
    echo ERROR: fragment.glsl not found
    dir Shaders\Source\
    pause
    exit /b 1
)

echo === Shaders compiled successfully ===
pause