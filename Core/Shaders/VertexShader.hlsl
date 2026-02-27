cbuffer CBuf : register(b0)
{
    matrix transform;
    float3 cubeColor;
};

struct VS_INPUT
{
    float3 pos : POSITION; // 12 tavua
    float3 normal : NORMAL; // 12 tavua
    float3 color : COLOR; // 12 tavua
    float brightness : BRIGHTNESS; // 4 tavua
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float3 color : COLOR0;
    float brightness : TEXCOORD2;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    output.pos = mul(float4(input.pos, 1.0f), transform);
    output.worldPos = input.pos;
    output.normal = normalize(input.normal);
    output.color = input.color; // Käytä meshin omaa väriä
    output.brightness = input.brightness;

    return output;
}