#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PixelIn input) :SV_Target0
{
    if (UseTexture(useOpacity))
    {
        clip(OpacityMap.Sample(sam, input.uv).a - 1);
    }
    return float4(0,0,0,1);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Shadow
// CullType : CullNone
// DepthType : LessEqual
// BlendType : Default
// Sampler : Default
