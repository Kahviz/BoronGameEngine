struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 size : SIZE;
};

float4 main(PS_INPUT input) : SV_TARGET {
    return input.color;
}