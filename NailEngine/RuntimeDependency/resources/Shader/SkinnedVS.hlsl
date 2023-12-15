#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint4 indices : BLENDINDICES;
    float4 weight : BLENDWEIGHT;
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
    
    float4 posL = float4(0.f, 0.f, 0.f,0.f);
    float3 normal = float3(0.f, 0.f, 0.f);
    float3 tangent = float3(0.f, 0.f, 0.f);
    
    for (int i = 0; i < 4; ++i)
    {
        posL += input.weight[i] * mul(float4(input.pos, 1.f), BoneTransform[input.indices[i]]);
        normal += input.weight[i] * mul(input.normal, (float3x3) BoneTransform[input.indices[i]]);
        tangent += input.weight[i] * mul(input.tangent, (float3x3) BoneTransform[input.indices[i]]);
    }
    
    //float3 temp0 = mul(input.indices[0] != 0xffffffffu, mul(mul(float4(input.pos, 1.f), BoneTransform[input.indices[0]]), input.weight[0])).xyz;
    //float3 temp1 = mul(input.indices[1] != 0xffffffffu, mul(mul(float4(input.pos, 1.f), BoneTransform[input.indices[1]]), input.weight[1])).xyz;
    //float3 temp2 = mul(input.indices[2] != 0xffffffffu, mul(mul(float4(input.pos, 1.f), BoneTransform[input.indices[2]]), input.weight[2])).xyz;
    //float3 temp3 = mul(input.indices[3] != 0xffffffffu, mul(mul(float4(input.pos, 1.f), BoneTransform[input.indices[3]]), input.weight[3])).xyz;
    //float3 temp4 = mul(input.indices[0] == 0xffffffffu, input.pos).xyz;
    
    //posL = temp0 + temp1 + temp2 + temp3 + temp4;
    
    //float4x4 tempW =
    //{
    //    0.1, 0, 0, 0,
    //    0, 0.1, 0, 0,
    //    0, 0, 0.1, 0,
    //    0, 0, 30, 1
    //};
    
    //float4x4 tempV =
    //{
    //    1, 0, 0, 0,
    //    0, 1, 0, 0,
    //    0, 0, 1, 0,
    //    0, 0, 10, 1
    //};
    
    //float4x4 tempP =
    //{
    //    1.357995, 0, 0, 0,
    //    0, 2.4142134, 0, 0,
    //    0, 0, 1.0000001, 1,
    //    0, 0, -1.0000001, 0
    //};
    
    //float4x4 result = mul(tempW, tempV);
    //result = mul(result, tempP);
    
    //output.posH = mul(float4(posL, 1.f), result);
    output.posH = mul(posL, WVP);
    output.posV = mul(posL, WV);
    output.color = input.color;
    output.uv = input.uv;
    output.normalV = normalize(mul(float4(normal, 0.f), WV));
    output.tangentV = normalize(mul(float4(tangent, 0.f), WV));
    output.biNormalV = normalize(cross(output.tangentV, output.normalV));
    
    return output;
}