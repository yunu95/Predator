#define RADIAL_FILL 1
#define RADIAL_FILL_CLOCKWISE 1 << 2
#define RADIAL_FILL_ORIGIN_UP 1 << 3
#define RADIAL_FILL_ORIGIN_RIGHT 1 << 4
#define RADIAL_FILL_ORIGIN_BOTTOM 1 << 5
#define RADIAL_FILL_ORIGIN_LEFT 1 << 6
Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

cbuffer uiImageCbuffers : register(b0)
{
    int flags[2000];
    float radius[2000];
};
float4 main(float4 color : COLOR0,
    float2 texCoord : TEXCOORD0) : SV_TARGET
{
    float2 texCoordWithZeroCenter = texCoord - 0.5;
    float radian = atan2(texCoordWithZeroCenter.y, texCoordWithZeroCenter.x);
    float radianThresHold = 0;
    // TODO
    return color * Texture.Sample(TextureSampler, texCoord);
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default