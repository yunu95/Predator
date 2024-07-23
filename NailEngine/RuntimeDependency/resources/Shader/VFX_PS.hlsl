#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 posH : SV_Position;
    float4 posW : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

// float0 : uv offset x
// float1 : uv offset y

float4 main(PixelIn input) : SV_TARGET
{
    float2 offset = float2(temp_float0, 1-temp_float1);
    float4 color = AlbedoMap.Sample(sam, input.uv + offset) * materialColor;
    color.a *= input.color.r;
    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : LessNoWrite
// BlendType : AlphaBlend
// Sampler : Default