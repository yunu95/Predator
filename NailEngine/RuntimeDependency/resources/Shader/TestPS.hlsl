#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

float main(PixelIn input) : SV_DEPTH
{
    return input.clipPos.z;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : LessEqual
// BlendType : Default
// Sampler : Default
