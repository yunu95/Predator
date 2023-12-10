#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 indices : INDICES;
    float4 weight : WEIGHT;
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
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    row_major matrix WV = mul(WTM, VTM);
    
    float3 posL = float3(0.f, 0.f, 0.f);
    float3 normal = float3(0.f, 0.f, 0.f);
    float3 tangent = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < 4; ++i)
    {
        posL += input.weight[i] * mul(float4(input.pos, 1.f), BoneTransform[input.indices[i]]).xyz;
        normal += input.weight[i] * mul(input.normal, (float3x3)BoneTransform[input.indices[i]]);
        tangent += input.weight[i] * mul(input.tangent, (float3x3) BoneTransform[input.indices[i]]);
    }
    
    output.posH = mul(float4(posL, 1.f), WVP);
    output.posV = mul(float4(posL, 1.f), WV);
    output.color = input.color;
    output.uv = input.uv;
    output.normalV = normalize(mul(float4(normal, 0.f), WV));
    output.tangentV = normalize(mul(float4(tangent, 0.f), WV));
    output.biNormalV = normalize(cross(output.tangentV, output.normalV));
    
    return output;
}