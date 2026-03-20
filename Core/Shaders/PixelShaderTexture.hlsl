Texture2D tex : register(t0);
Texture2D shadowMap : register(t2);
SamplerState samp : register(s0);
SamplerComparisonState shadowSamp : register(s1);

cbuffer ColorBuffer : register(b0)
{
    float4 color;
};

cbuffer LightingBuffer : register(b1)
{
    float3 lightPos;
    float brightness;
    float3 worldPos;
    float lightRange;
};

struct PS_INPUT
{
    float brightness : BRIGHTNESS;
    float4 pos : SV_POSITION;
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD;
    float4 lightSpacePos : LIGHT_POS;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, input.tex);
    
    float3 lightDir = normalize(lightPos - input.worldPos);
    float lightDist = length(lightPos - input.worldPos);
    
    float4 shadowCoord = input.lightSpacePos / input.lightSpacePos.w;
    shadowCoord.xy = shadowCoord.xy * 0.5 + 0.5;
    shadowCoord.y = 1.0 - shadowCoord.y;
    
    float shadowFactor = 1.0;
    if (shadowCoord.x > 0 && shadowCoord.x < 1 &&
        shadowCoord.y > 0 && shadowCoord.y < 1)
    {
        float currentDepth = shadowCoord.z;
        shadowFactor = shadowMap.SampleCmpLevelZero(shadowSamp, shadowCoord.xy, currentDepth);
    }
    
    float diff = max(dot(input.normal, lightDir), 0.2) * brightness * shadowFactor;
    
    return float4(texColor.rgb * diff, texColor.a);
}