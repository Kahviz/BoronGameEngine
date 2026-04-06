#version 450

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    float usesTexture;
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec3 fragNormal;

void main() {
    fragColor = inColor;
    fragUV = inUV;
    fragNormal = normalize(inNormal);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 1.0);
}