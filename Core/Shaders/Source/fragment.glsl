#version 450

vec3 lightDir = normalize(vec3(0.5, -1.0, 0.5));

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec4 fragPosLightSpace;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 color;
    float usesTexture;
    mat4 lightSpaceMatrix;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2DShadow shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0 ||
       projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0)
        return 1.0;

    float currentDepth = projCoords.z;

    float bias = max(0.005 * (1.0 - dot(normalize(fragNormal), -lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(2048.0);

    for(int x = -1; x <= 1; x++)
    for(int y = -1; y <= 1; y++)
    {
        vec2 offset = vec2(x, y) * texelSize;
        shadow += texture(shadowMap, vec3(projCoords.xy + offset, currentDepth - bias));
    }

    shadow /= 9.0;

    return mix(0.3, 1.0, shadow);
}

void main()
{
    float diff = max(dot(normalize(fragNormal), -lightDir), 0.0);

    float ambient = 0.2;
    float lighting = ambient + (1.0 - ambient) * diff;

    float shadow = ShadowCalculation(fragPosLightSpace);
    lighting *= shadow;

    vec4 baseColor;
    if (ubo.usesTexture > 0.5)
        baseColor = texture(texSampler, fragUV);
    else
        baseColor = vec4(ubo.color, 1.0);

    outColor = vec4(baseColor.rgb * lighting, baseColor.a);
}