#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(PixelIn input) : SV_Target0
{
    float4 materialTemp = materialColor;
    
    half dissolve_value = Temp0Map.Sample(sam, input.uv).r;
    clip(dissolve_value - temp_float0);
    
    if (UseTexture(useOpacity))
    {
        clip(OpacityMap.Sample(sam, input.uv).a - 1);
    }
    return float4(0, 0, 0, 1);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Shadow
// CullType : CullNone
// DepthType : Less
// BlendType : Default
// Sampler : Default
