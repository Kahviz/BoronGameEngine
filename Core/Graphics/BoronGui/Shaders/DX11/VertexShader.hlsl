struct VS_INPUT {
    float2 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 size : SIZE;
};

struct VS_OUTPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 size : SIZE;
};

VS_OUTPUT main(VS_INPUT input) {
    VS_OUTPUT output;

    output.pos = float4(input.pos, 0.0f, 1.0f);
    output.color = input.color;
    output.uv = input.uv;
    output.size = input.size;
    
    return output;
}