#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
};

struct VertexOut
{
    float4 posH : SV_Position;
    float3 posL : POSITION;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.posH = mul(float4(input.pos, 1.f), WVP).xyww;
    output.posL = input.pos;
    
    return output;
}