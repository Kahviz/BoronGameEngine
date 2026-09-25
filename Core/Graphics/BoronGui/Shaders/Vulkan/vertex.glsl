#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inUV;
layout(location = 3) in float inBrightness;
layout(location = 4) in vec2 inSize;
layout(location = 5) in float inRounding;
layout(location = 6) in vec2 inGuiPosition;
layout(location = 7) in uint inTextureID;

layout(push_constant) uniform PushConstants {
    vec2 viewportSize;
} globalPC;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out float fragRounding;
layout(location = 2) out vec2 fragLocalPos;
layout(location = 3) out vec2 fragSize;
layout(location = 4) out vec2 uv;
layout(location = 5) out flat uint textureID;

void main() {
    vec2 pixelPosition = inGuiPosition +(inPosition + vec2(0.5)) * inSize;

    vec2 ndc;
    ndc.x = (pixelPosition.x / globalPC.viewportSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (pixelPosition.y / globalPC.viewportSize.y) * 2.0;

    gl_Position = vec4(ndc, 0.0, 1.0);

    vec2 localPos = (inPosition + vec2(0.5)) * inSize;

    fragLocalPos = localPos;
    fragRounding = inRounding;
    fragColor = inColor;
    fragSize = inSize;
    uv = inUV;
    textureID = inTextureID;
}