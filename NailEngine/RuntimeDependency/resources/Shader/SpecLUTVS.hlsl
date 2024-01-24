#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct VertexOut
{
    float4 posH : SV_Position;
    float3 posL : POSITION;
    float2 uv : TEXCOORD0;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.posH = mul(float4(input.pos, 1.f), WVP).xyww;
    output.posL = input.pos;
    output.uv = input.uv;
    
    return output;
}