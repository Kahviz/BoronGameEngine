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
};

float4 main(PS_INPUT input) : SV_Target
{
    float3 N = normalize(input.normal);
    float3 lightDir = normalize(lightpos - input.worldPos);

    float diffuse = max(dot(N, lightDir), 0.0);
    float ambient = 0.2;

    float dist = distance(input.worldPos, lightpos);
    float attenuation = saturate(1.0 - dist / lightRange);

    float lightAmount = ambient + diffuse * 0.2 * attenuation;
    lightAmount = saturate(lightAmount);

    float3 finalColor = input.color * lightAmount;
    return float4(finalColor, 1.0f);
}
