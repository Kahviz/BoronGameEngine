cbuffer CBuf : register(b0)
{
    matrix transfrom; // WorldViewProjection-matriisi
    float3 cubeColor; // Objektin väri
};

cbuffer LightingCB : register(b1)
{
    float3 lightpos; // Valon sijainti maailmakoordinaatistossa
    float Brightness;
    float3 WorldPos;
    float lightRange;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // Clip-space positio
    float3 worldPos : TEXCOORD0; // Maailmakoordinaatit
    float3 normal : TEXCOORD1; // Normaali
    float3 color : COLOR0; // Base väri
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.pos = mul(float4(input.pos, 1.0f), transfrom);

    output.worldPos = input.pos;
    output.normal = normalize(input.normal);

    output.color = cubeColor;

    return output;
}