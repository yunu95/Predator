#include "Buffers.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    
    // INSTANCING
    uint instanceID : SV_INSTANCEID;
    row_major matrix world : INST;
};

struct VS_OUT
{
    float4 pos : POSITION;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.pos = mul(float4(input.pos, 1.f), input.world);

    return output;
}