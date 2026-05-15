Texture2D tex : register(t0);
Texture2D shadowMap : register(t2);
SamplerState samp : register(s0);
SamplerComparisonState shadowSamp : register(s1);

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

float ShadowCalculation(float4 fragPosLightSpace, float3 fragNormal, float3 lightDir)
{
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0 ||
        projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0)
        return 1.0;
    
    float currentDepth = projCoords.z;
    
    float bias = max(0.005 * (1.0 - dot(normalize(fragNormal), -lightDir)), 0.52);
    
    float shadow = 0.0;
    float2 texelSize = 1.0 / float2(2048.0, 2048.0);
    
    // 3x3 PCF
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float2 offset = float2(x, y) * texelSize;
            shadow += shadowMap.SampleCmpLevelZero(
                shadowSamp,
                projCoords.xy + offset,
                currentDepth - bias
            );
        }
    }
    
    shadow /= 9.0;
    
    return lerp(0.3, 1.0, shadow);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 texColor = tex.Sample(samp, input.tex);
    
    float3 lightDir = normalize(lightpos - input.worldPos);
    float3 normal = normalize(input.normal);
    
    float diffuse = max(0.3f, dot(normal, lightDir));
    
    float shadowFactor = ShadowCalculation(input.lightSpacePos, input.normal, lightDir);
    
    float3 finalColor = texColor.rgb * diffuse * shadowFactor;
    
    return float4(finalColor, texColor.a);
}