@echo off

set GLSLANG=C:\VulkanSDK\1.4.335.0\Bin\glslangValidator.exe

%GLSLANG% -V vertex.glsl ^
    --target-env vulkan1.2 ^
    -S vert ^
    --vn VertexShader ^
    -o VertexShader.h

%GLSLANG% -V fragment.glsl ^
    --target-env vulkan1.2 ^
    -S frag ^
    --vn FragmentShader ^
    -o FragmentShader.h

(
    echo #include "cstdint"
    type VertexShader.h
) > VertexShader.tmp
move /Y VertexShader.tmp VertexShader.h >nul

(
    echo #include "cstdint"
    type FragmentShader.h
) > FragmentShader.tmp
move /Y FragmentShader.tmp FragmentShader.h >nul

echo Compiled!
pause