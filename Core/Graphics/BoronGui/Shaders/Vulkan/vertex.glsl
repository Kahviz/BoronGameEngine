#version 450

#extension GL_KHR_vulkan_glsl : enable

layout(location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstants
{
    vec4 color;
} pc;

vec2 positions[3] = vec2[](
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = pc.color;
}