#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut) 0;
    
    output.pos = mul(float4(input.pos, 1.f), WVP);
    output.uv = input.uv;
    
    return output;
}