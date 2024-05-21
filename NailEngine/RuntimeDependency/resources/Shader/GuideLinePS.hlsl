#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 posV : SV_Position;
    float4 posW : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 tangentV : TANGENT;
    float3 biNormalV : BINORMAL;
};

float4 main(PixelIn input) : SV_TARGET
{
    float4 color = Temp0Map.Sample(sam, input.uv);
    color.xyz *= materialColor.xyz;
    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : LessNoWrite
// BlendType : AlphaBlend
// Sampler : Default