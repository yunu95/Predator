#include "Buffers.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float2 lightUV : TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    // INSTANCING
    uint instanceID : SV_INSTANCEID;
};

struct VS_OUT
{
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD0;
    uint id : ID;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    float3 worldPos = mul(float4(input.pos, 1.f), WTM).xyz;
    worldPos += particleDesc[input.instanceID].pos;

    output.viewPos = mul(float4(worldPos, 1.f), VTM);
    output.uv = input.uv;
    output.id = input.instanceID;

    return output;
}