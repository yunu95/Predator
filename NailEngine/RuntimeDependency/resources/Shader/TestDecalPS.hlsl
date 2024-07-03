#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 posH : SV_Position;
    float4 posW : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 tangentV : TANGENT;
    float3 biNormalV : BINORMAL;
};

// Temp0 : 맵핑할 텍스처
// Temp1 : View Space에서의 Position

float2 GetPixelUV(float4 pos)
{
    float2 uv = pos.xy / pos.w;
    uv = uv * float2(0.5, -0.5) + 0.5;
    return uv;
}

float3 GetPixelLocal(float2 uv, float depth)
{
    float3 proj = float3(lerp(-projInvVec.xy, projInvVec.xy, uv), projInvVec.z);
    
    float4 pos = float4(proj * depth, 1.f);
    
    float3 localpos = mul(pos, mul(VTMInv, inverse(WTM)));

    return localpos;
}

float4 main(PixelIn input) : SV_TARGET
{
    float2 tempUV = GetPixelUV(input.posW);
    float4 depth = Temp1Map.Sample(sam, tempUV);
    float3 decalLocalPos = GetPixelLocal(tempUV, depth.z);
    float4 color = float4(0, 0, 0, 1);
    
    clip(0.5 - abs(decalLocalPos.x));
    clip(0.5 - abs(decalLocalPos.y));
    clip(0.5 - abs(decalLocalPos.z));
    
    
    float2 decalUV = decalLocalPos.xz + 0.5;
    color = Temp0Map.Sample(sam, decalUV);
    color *= materialColor;
    
    return color;
}

// ShaderInfo
// ShaderType : Decal
// RasterType : Solid
// CullType : CullBack
// DepthType : LessNoWrite
// BlendType : AlphaBlend
// Sampler : Default