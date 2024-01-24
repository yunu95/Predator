#include "Buffers.hlsli"

struct PixelIn
{
    float4 posH : SV_Position;
    float3 posL : POSITION;
};

float4 main(PixelIn input) : SV_TARGET
{
    return CubeMap.Sample(sam, input.posL);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullFront
// DepthType : LessEqual
// BlendType : Default
// Sampler : Default