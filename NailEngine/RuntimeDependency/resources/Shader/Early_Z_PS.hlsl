#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 posH : SV_Position;
    float4 posV : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 tangentV : TANGENT;
    float3 biNormalV : BINORMAL;
    float2 lightUV : TEXCOORD1;
    uint id : ID;
};

float4 main(PixelIn input) : SV_Target
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    float4 p_pos = mul(input.posV, PTM);
    float depth = p_pos.z / p_pos.w;
    return float4(depth,depth,depth,1);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : LessNoWrite
// BlendType : Default
// Sampler : Default
