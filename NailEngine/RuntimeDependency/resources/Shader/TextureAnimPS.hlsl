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
    float2 offset = float2(-temp_float0, temp_float1);
    float4 color = Temp0Map.Sample(sam, input.uv + offset);
    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : NoDepthTestNoWrite
// BlendType : AlphaBlend
// Sampler : Default