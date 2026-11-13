cbuffer Viewport : register(b0) {
    float2 viewportSize;
    float2 padding;
};

struct VS_INPUT {
    float2 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 size : SIZE;
    float2 guiPos : GUIPOS;
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 size : SIZE;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    float2 local = input.pos + 0.5f;
    float2 px = input.guiPos + local * input.size;

    float2 vp = max(viewportSize, float2(1.0f, 1.0f));
    float2 ndc = (px / vp) * 2.0f - 1.0f;

    output.pos = float4(ndc, 0.0f, 1.0f);
    output.color = input.color;
    output.uv = input.uv;
    output.size = input.size;
    return output;
}