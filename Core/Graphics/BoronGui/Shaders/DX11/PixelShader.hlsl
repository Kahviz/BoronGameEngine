Texture2D textures[16] : register(t0);
SamplerState textureSampler : register(s0);

struct PS_INPUT {
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 size : SIZE;
    float rounding : ROUNDING;
    nointerpolation uint textureID : TEXTUREID;
};

float RoundedBoxSDF(float2 centerPos, float2 halfSize, float radius) {
    float2 q = abs(centerPos) - halfSize + radius;

    return length(max(q, 0.0f)) + min(max(q.x, q.y), 0.0f) - radius;
}

float4 main(PS_INPUT input) : SV_TARGET {
    float4 texColor = textures[0].Sample(textureSampler, input.uv);

    float2 centerPos = (input.uv - 0.5f) * input.size;

    float2 halfSize = input.size * 0.5f;

    float distance = RoundedBoxSDF(centerPos, halfSize, input.rounding);

    float alpha = 1.0f - smoothstep(0.0f, 1.0f, distance);

    return texColor;
}