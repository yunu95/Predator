#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

void main(PixelIn input) 
{
    return;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Shadow
// CullType : CullBack
// DepthType : LessEqual
// BlendType : Default
// Sampler : Default
