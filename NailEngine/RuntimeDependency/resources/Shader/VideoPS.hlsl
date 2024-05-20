//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float4 Color : COLOR;
};

//--------------------------------------------------------------------------------------
Texture2D txYUV : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float3 YUVToRGB(float3 yuv)
{
  // BT.601 coefs
    static const float3 yuvCoef_r = { 1.164f, 0.000f, 1.596f };
    static const float3 yuvCoef_g = { 1.164f, -0.392f, -0.813f };
    static const float3 yuvCoef_b = { 1.164f, 2.017f, 0.000f };
    yuv -= float3(0.0625f, 0.5f, 0.5f);
    return saturate(float3(
    dot(yuv, yuvCoef_r),
    dot(yuv, yuvCoef_g),
    dot(yuv, yuvCoef_b)
    ));
}

float4 main(VS_OUTPUT input) : SV_Target
{
    float y = txYUV.Sample(samLinear, float2(input.Tex.x, input.Tex.y * 0.5)).r;
    float u = txYUV.Sample(samLinear, float2(input.Tex.x * 0.5, 0.50 + input.Tex.y * 0.25)).r;
    float v = txYUV.Sample(samLinear, float2(input.Tex.x * 0.5, 0.75 + input.Tex.y * 0.25)).r;
    return float4(YUVToRGB(float3(y, u, v)), 1.f);
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default