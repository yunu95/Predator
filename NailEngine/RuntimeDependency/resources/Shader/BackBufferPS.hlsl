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
            
    //color *= 0.7f;
    //float3 x = max(0, color.xyz - 0.004);
    //color.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    color.w = 1;
    
    
    float4 viewPos = Temp1Map.Sample(sam, input.uv);
    
    float fogFactor = saturate((fogEnd - (viewPos.z/ viewPos.w)) / (fogEnd - fogStart));
    float4 fogColor = fogFactor * color + (1 - fogFactor) * float4(0.7686, 0.8784, 0.9451,1.f);
    //return fogColor;
    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default