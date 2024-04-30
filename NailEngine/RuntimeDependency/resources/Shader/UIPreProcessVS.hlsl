#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 clippingDirection : TEXCOORD1;
    float clippingThreshold : TEXCOORD2;
    float2 linearClippingDirection : TEXCOORD3;
    float2 linearClippingStart : TEXCOORD4;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float2 clippingDirection : TEXCOORD1;
    float clippingThreshold : TEXCOORD2;
    float2 linearClippingDirection : TEXCOORD3;
    float2 linearClippingStart : TEXCOORD4;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    output.uv = input.uv;
    output.pos = float4(input.pos, 1);
    output.clippingDirection = input.clippingDirection;
    output.clippingThreshold = input.clippingThreshold;
    output.linearClippingDirection = input.linearClippingDirection;
    output.linearClippingStart = input.linearClippingStart;
    return output;
}
