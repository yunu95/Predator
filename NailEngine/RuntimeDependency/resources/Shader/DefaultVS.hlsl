#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float4 posW : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biNormal : BINORMAL;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.pos = mul(float4(input.pos, 1.f), WVP);
    output.posW = mul(float4(input.pos, 1.f), WTM);
    output.color = input.color;
    output.uv = input.uv;
    output.normal = normalize(mul(float4(input.normal,0.f), WTM));
    output.tangent = normalize(mul(float4(input.tangent, 0.f), WTM));
    output.biNormal = normalize(cross(output.tangent,output.normal));
    
    return output;
}