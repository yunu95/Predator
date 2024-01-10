#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
};

float4 main(PixelIn input) : SV_Target0
{
    return float4(input.clipPos.z, input.clipPos.z, input.clipPos.z, 0.f);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : NoDepthTest
// BlendType : Default
