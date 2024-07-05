#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float2 lightUV : TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexOut
{
    float4 posH : SV_Position;
    float4 posW : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 tangentV : TANGENT;
    float3 biNormalV : BINORMAL;
};

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut) 0;
    
    row_major matrix VP = mul(VTM, PTM);
    
    output.posH = mul(float4(input.pos, 1.f), WTM);
    output.posH = mul(output.posH, VP);
    output.posW = mul(float4(input.pos, 1.f), mul(WTM, mul(VTM, PTM)));
    
    output.uv = input.uv;
    return output;
}