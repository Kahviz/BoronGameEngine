cbuffer ConstantBuffer : register(b0)
{
    matrix worldViewProj;
    matrix world;
    float3 cubeColor;
    float padding;
};

cbuffer ShadowCB : register(b1)
{
    matrix lightViewProj;
    float3 lightPos;
    float padding2;
};

struct VS_INPUT
{
    float brightness : BRIGHTNESS;
    float3 pos : POSITION;
    float3 col : COLOR;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : COLOR0;
    float brightness : TEXCOORD2;
    float2 tex : TEXCOORD3;
    float4 lightSpacePos : LIGHT_POS;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    output.worldPos = worldPos.xyz;
    
    output.pos = mul(float4(input.pos, 1.0f), worldViewProj);
    
    output.lightSpacePos = mul(worldPos, lightViewProj);

    float3x3 worldRotation = (float3x3) world;
    output.normal = normalize(mul(input.norm, worldRotation));
    
    output.brightness = input.brightness;
    output.color = input.col;
    output.tex = input.tex;
    
    return output;
}