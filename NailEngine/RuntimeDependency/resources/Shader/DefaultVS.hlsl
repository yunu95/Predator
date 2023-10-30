#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.pos = mul(float4(input.pos, 1.f), WVP);
    
    output.color = input.color;
    output.uv = input.uv;
    return output;
}