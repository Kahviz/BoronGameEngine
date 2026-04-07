#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 color;
    float usesTexture;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
    vec4 maincolor;
    if (ubo.usesTexture > 0.5) {
        //Texture
        outColor = texture(texSampler, fragUV);
    } else {
        //No Texture
        outColor = vec4(ubo.color, 1.0);
    }
}
/*
void main() {
    vec4 maincolor;
    if (ubo.usesTexture > 0.5) {
        //Texture
        outColor = texture(texSampler, fragUV);
    } else {
        //No Texture
        outColor = vec4(ubo.color, 1.0);
    }
}
*/