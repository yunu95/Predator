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

// temp_int_9 : 오브젝트 인덱스ID


float4 main(PixelIn input) : SV_TARGET
{
    float4 color = float4(temp_int9, input.uv.x, input.uv.y, 1.f);
    
    return color;
}

// ShaderInfo
// ShaderType : Decal
// RasterType : Solid
// CullType : CullBack
// DepthType : LessNoWrite
// BlendType : Default
// Sampler : Default