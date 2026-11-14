struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 size : SIZE;
};

float RoundedBoxSDF(float2 centerPos, float2 halfSize, float radius) {
    float2 q = abs(centerPos) - halfSize + radius;

    return length(max(q, 0.0f))
        + min(max(q.x, q.y), 0.0f)
        - radius;
}

float4 main(PS_INPUT input) : SV_TARGET {
    return input.color;
}