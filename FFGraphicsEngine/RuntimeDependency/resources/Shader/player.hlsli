#ifndef _PLAYER_HLSLI_
#define _PLAYER_HLSLI_

#include "params.hlsli"
#include "utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 weight : WEIGHT;
    float4 indices : INDICES;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
    row_major matrix matProj : P;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);
    
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;
    
    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
    output.color = g_color;
    
    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    float4 depth : SV_Target3;
};

PS_OUT PS_Main(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;
    
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    if (g_tex_on_0 == 1)
    {
        color = g_tex_0.Sample(g_sam_0, input.uv);
        color.rgb = pow(color.rgb, 2.2);
    }
    
    clip(color.a - 0.1f);
    
    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1 == 1)
    {
        // [0, 255] 범위에서 [0, 1]로 변환
        float3 tangentSpaceNormal = pow(g_tex_1.Sample(g_sam_0, input.uv).xyz, 1 / 2.2f);
        //float3 tangentSpaceNormal = pow(g_tex_1.Sample(g_sam_0, input.uv).xyz, 2.2f);
        //float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        
        // [0, 1] 범위에서 [-1, 1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }
    
    output.position = float4(input.viewPos.xyz, 0.f);
    output.normal = float4(viewNormal.xyz, 0.f);
    output.color = color * g_color;
    //output.color = color;
    
    float4 projPos = { 0, 0, 0, 0 };
    
    projPos = mul(float4(input.viewPos, 1.f), g_matProjection);
    
    float depth = projPos.z / projPos.w;
    
    //output.depth = float4(depth, depth, depth, depth);
   if (g_float_3 == 1.f)
   {
       output.depth = float4(1.f, 1.f, 1.f, 1.f);
   }
   else
   {
       output.depth = float4(0.f, 0.f, 0.f, 0.f);
   }
    
    
    return output;
}

#endif