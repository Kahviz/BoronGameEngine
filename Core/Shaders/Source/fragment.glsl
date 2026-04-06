#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    float usesTexture;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
    vec4 maincolor = { 1.0, 0.0, 0.0, 1.0};

    if (ubo.usesTexture >= 0.5) {
        maincolor = texture(texSampler, fragUV);
    }
    else {
        maincolor = vec4(fragColor,1.0);
    }

    vec3 normalColor = fragNormal * 0.5 + 0.5;
    
    outColor = vec4(maincolor);
}