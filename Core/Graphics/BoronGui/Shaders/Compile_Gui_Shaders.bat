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

powershell -Command "$content = Get-Content VertexShader.h; @('#include <cstdint>') + $content | Set-Content VertexShader.h"
powershell -Command "$content = Get-Content FragmentShader.h; @('#include <cstdint>') + $content | Set-Content FragmentShader.h"
echo Compiled!
pause