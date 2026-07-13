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


#define AMBIENT 0.2


float ShadowCalculation(vec4 shadowCoord)
{
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    if (projCoords.z > 1.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 1.0;
    }


    float bias = 0.0003;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);


    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            shadow += texture(
                shadowMap,
                vec3(
                    projCoords.xy + vec2(x,y) * texelSize,
                    projCoords.z - bias
                )
            );
        }
    }


    return shadow / 9.0;
}


void main()
{
    vec3 normal = normalize(fragNormal);
    vec3 light = normalize(-lightDir);

    float diff = max(dot(normal, light), 0.0);
    float shadow = ShadowCalculation(fragPosLightSpace);
    float lighting =
        AMBIENT +
        (1.0 - AMBIENT) * diff * shadow;

    vec4 baseColor =
        texture(texSampler, fragUV);

    outColor = vec4(
        baseColor.rgb * lighting,
        baseColor.a
    );
}