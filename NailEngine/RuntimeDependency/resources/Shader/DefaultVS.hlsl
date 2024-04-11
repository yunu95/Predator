#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    // INSTANCING
    uint instanceID : SV_INSTANCEID;
    row_major matrix world : INST;
};

struct VertexOut
{
    float4 posH : SV_Position;
    float4 posV : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 tangentV : TANGENT;
    float3 biNormalV : BINORMAL;
    float2 lightUV : TEXCOORD1;
    uint id : ID;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    row_major matrix WV = mul(input.world, VTM);
    row_major matrix VP = mul(VTM, PTM);
    
    output.posH = mul(float4(input.pos, 1.f), input.world);
    output.posH = mul(output.posH, VP);
    output.posV = mul(float4(input.pos, 1.f), WV);
    output.color = input.color;
    
    float2 tempUV = input.uv;
    tempUV.x *= lightMapUV[input.instanceID].scaling.x;
    tempUV.y = (1 - tempUV.y);
    tempUV.y *= (lightMapUV[input.instanceID].scaling.y);
    tempUV.y = (1 - tempUV.y);
    
    tempUV.x += (lightMapUV[input.instanceID].uvOffset.x);
    tempUV.y += (-lightMapUV[input.instanceID].uvOffset.y);
    
    output.uv = input.uv;
    output.lightUV = tempUV;
    output.normalV = normalize(mul(float4(input.normal, 0.f), WV));
    output.tangentV = normalize(mul(float4(input.tangent, 0.f), WV));
    output.biNormalV = normalize(cross(output.tangentV, output.normalV));
    output.id = input.instanceID;
    
    return output;
}