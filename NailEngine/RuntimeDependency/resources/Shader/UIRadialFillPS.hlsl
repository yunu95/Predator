Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float2 clippingDirection : TEXCOORD1;
    float clippingThreshold : TEXCOORD2;
};

float4 main(PixelIn input) : SV_TARGET
{
    clip(dot(normalize(float2(input.uv.x - 0.5, (1 - input.uv.y) - 0.5)), normalize(input.clippingDirection)) - input.clippingThreshold);
    // TODO
    return Texture.Sample(TextureSampler, input.uv);
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default