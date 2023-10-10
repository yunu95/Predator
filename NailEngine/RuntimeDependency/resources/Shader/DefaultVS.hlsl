#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.pos = mul(float4(input.pos, 1.f), WVP);
    
    output.color = input.color;
    
    return output;
}