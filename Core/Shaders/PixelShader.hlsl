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
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 lightDir = normalize(float3(1, 1, -1));
    float3 normal = normalize(input.normal);
    
    float diffuse = max(0.3f, dot(normal, lightDir));
    
    float3 finalColor = input.color * diffuse * (input.brightness / 2);
    
    return float4(finalColor, 1.0f);
}