#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// BackBuffer
// Temp0Map : Final Target
// Temp1Map : View Position

float4 main(PixelIn input) : SV_Target
{
    float4 color = Temp0Map.Sample(sam, input.uv);
    float4 viewPos = Temp1Map.Sample(sam, input.uv);
    
    float fogFactor = saturate((fogEnd - (viewPos.z/ viewPos.w)) / (fogEnd - fogStart));
    float4 fogColor = fogFactor * color + (1 - fogFactor) * float4(0.7686, 0.8784, 0.9451,1.f);
    return fogColor;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default