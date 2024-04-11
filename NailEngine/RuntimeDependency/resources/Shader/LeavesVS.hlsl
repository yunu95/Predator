#include "Buffers.hlsli"
#include "Matrix.hlsli"

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
    VertexOut output = (VertexOut) 0;
    
    float2 lightuv = input.uv;
    lightuv.x *= lightMapUV[input.instanceID].scaling.x;
    lightuv.y = (1 - lightuv.y);
    lightuv.y *= (lightMapUV[input.instanceID].scaling.y);
    lightuv.y = (1 - lightuv.y);
    
    lightuv.x += (lightMapUV[input.instanceID].uvOffset.x);
    lightuv.y += (-lightMapUV[input.instanceID].uvOffset.y);
    
    output.lightUV = lightuv;
    
    input.uv = input.uv - floor(input.uv);
   
    
    row_major matrix WV = mul(input.world, VTM);
    row_major matrix VP = mul(VTM, PTM);
    
    float3 scale;
    float3 pos;
    float4 quat;
    decompose(input.world,pos, quat,scale);
    
    output.posH = mul(float4(input.pos, 1.f), input.world);
     ///
    float2 tempUV = input.uv;
    tempUV.y = 1 - tempUV.y;
    
    tempUV.x = 1 - tempUV.x;
    
    tempUV *= 2;
    tempUV -= 1;
    tempUV.y *= 1;
    tempUV.x *= -1;
    float4 tempVec = float4(tempUV, 0, 0);
    tempVec = mul(tempVec, VTMInv);
    tempVec = normalize(tempVec);
    tempVec *= scale.y;
    
    //float3 tempNormal = mul(float4(input.normal, 0), input.world);
    //tempNormal *= 0.5f;
    //float3 tempPos = (tempVec.xyz + tempNormal) * 1.1f;
    float3 tempPos = (tempVec.xyz) * 1.1f;
    output.posH.xyz += tempPos;
    
    ///
    output.posH = mul(output.posH, VP);
    output.posV = mul(float4(input.pos, 1.f), WV);
    output.color = input.color;
    output.uv = input.uv;
    output.normalV = normalize(mul(float4(input.normal, 0.f), WV));
    output.tangentV = normalize(mul(float4(input.tangent, 0.f), WV));
    output.biNormalV = normalize(cross(output.tangentV, output.normalV));
    output.id = input.instanceID;
    return output;
}