#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

float4 main(PixelIn input) : SV_TARGET
{
    return Temp0Map.Sample(sam, input.uv);
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default