#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in float fragRounding;
layout(location = 2) in vec2 fragLocalPos;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = fragColor; //fragColor;
}