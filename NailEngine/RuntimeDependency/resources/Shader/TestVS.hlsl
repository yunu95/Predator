#include "Buffers.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float2 lightUV : TEXCOORD1;
    
    // INSTANCING
    uint instanceID : SV_INSTANCEID;
    row_major matrix world : INST;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.pos = mul(float4(input.pos, 1.f), input.world);
    output.pos = mul(output.pos, mul(VTM, PTM));
    output.clipPos = output.pos;
    output.uv = input.uv;
    
    return output;
}