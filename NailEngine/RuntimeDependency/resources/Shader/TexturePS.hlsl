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
    return Temp0Map.Sample(sam, input.uv);
}

// ShaderInfo
// ShaderType : Solid
