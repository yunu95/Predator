#ifndef _PORTAL_HLSLI_
#define _PORTAL_HLSLI_

#include "params.hlsli"
#include "utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

// g_float_0 : time
// g_float_1 : speed

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    //output.color = input.color;
    output.uv = input.uv + float2(0.5f,0.5f);
    
    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));
        
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    
    if(g_tex_on_0)
    {
        color = g_tex_0.Sample(g_sam_0, input.uv);
    }
    
    color = color;
    return color;
}
#endif