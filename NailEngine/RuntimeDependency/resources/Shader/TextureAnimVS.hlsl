#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    
     // INSTANCING
    uint instanceID : SV_INSTANCEID;
    row_major matrix world : INST;
};

struct VertexOut
{
    float4 posH : SV_Position;
    float4 posW : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut) 0;
    
    row_major matrix VP = mul(VTM, PTM);
    
    output.posH = mul(float4(input.pos, 1.f), input.world);
    output.posH = mul(output.posH, VP);
    output.color = input.color;
    output.uv = input.uv;
    return output;
}