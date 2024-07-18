#include "Buffers.hlsli"

struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    uint id : SV_InstanceID;
};

float4 main(GS_OUT input) : SV_TARGET
{
    float4 color = AlbedoMap.Sample(sam, input.uv);
    color.a *= particleDesc[input.id].alpha;
    return color;
}

// ShaderInfo
// ShaderType : Particle
// RasterType : Solid
// CullType : CullBack
// DepthType : LessNoWrite
// BlendType : AlphaBlend
// Sampler : Default