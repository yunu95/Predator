#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.pos = mul(float4(input.pos, 1.f), WVP);
    output.color = input.color;
    output.uv = input.uv;
    output.normal = normalize(mul(float4(input.normal,0.f), WTM));
    
    float4 worldPosition = mul(float4(input.pos, 1.f), WTM);
    
    output.viewDirection = normalize(cameraPos.xyz - worldPosition.wxz);
    
    return output;
}