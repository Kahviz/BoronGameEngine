cbuffer CBuf : register(b0)
{
    matrix transform;
    float3 cubeColor;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
    float brightness : BRIGHTNESS;
    float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : COLOR0;
    float brightness : TEXCOORD2;
    float2 tex : TEXCOORD3;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.pos = mul(float4(input.pos, 1), transform);
    output.worldPos = input.pos;
    output.normal = normalize(input.normal);
    output.color = input.color;
    output.brightness = input.brightness;
    output.tex = input.tex;

    return output;
}