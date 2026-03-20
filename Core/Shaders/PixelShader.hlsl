cbuffer ColorBuffer : register(b0)
{
    float4 color;
};

cbuffer LightingCB : register(b1)
{
    float3 lightpos;
    float Brightness;
    float3 WorldPos;
    float lightRange;
};

Texture2D shadowMap : register(t2);
SamplerState samplerState : register(s0);
SamplerComparisonState shadowSampler : register(s1);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : COLOR0;
    float brightness : TEXCOORD2;
    float2 tex : TEXCOORD3;
    float4 lightSpacePos : LIGHT_POS;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 baseColor = input.color;
    
    float3 lightDir = normalize(lightpos - input.worldPos);
    float3 normal = normalize(input.normal);
    
    float diffuse = max(0.3f, dot(normal, lightDir));
    
    float shadowFactor = 1.0f;
    
    float4 shadowCoord = input.lightSpacePos;
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.x = shadowCoord.x * 0.5f + 0.5f;
    shadowCoord.y = -shadowCoord.y * 0.5f + 0.5f;
    
    if (shadowCoord.x > 0.0f && shadowCoord.x < 1.0f &&
        shadowCoord.y > 0.0f && shadowCoord.y < 1.0f &&
        shadowCoord.z > 0.0f && shadowCoord.z < 1.0f)
    {
        float bias = 0.005f;
        shadowFactor = shadowMap.SampleCmpLevelZero(
            shadowSampler,
            shadowCoord.xy,
            shadowCoord.z - bias
        );
    }
    
    float3 finalColor = baseColor * diffuse * shadowFactor;
    
    return float4(finalColor, 1.0f);
}