#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// BackBuffer
// Temp0Map : Final Target

float4 main(PixelIn input) : SV_Target
{
    return Temp0Map.Sample(sam,input.uv);
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default