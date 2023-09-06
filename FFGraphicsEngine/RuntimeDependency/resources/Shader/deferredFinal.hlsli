#ifndef _LIGHTING_HLSLI_
#define _LIGHTING_HLSLI_

#include "params.hlsli"
#include "utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// [Final]
// g_tex_0 : Diffuse Color Target
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
// Mesh : Rectangle

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    
    output.pos = float4(input.pos * 2, 1.f);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Final(VS_OUT input) : SV_Target
{
    float4 output = (float4)0;
    
    float4 lightPower = g_tex_1.Sample(g_sam_0, input.uv);
    if(lightPower.x == 0.f && lightPower.y == 0.f && lightPower.z == 0.f)
    {
        clip(-1);
    }

    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    float4 specular = g_tex_2.Sample(g_sam_0, input.uv);
    
    output = (color * lightPower) + specular;
    output.rgb = pow(output.rgb, 1 / 2.2);
    return output;
}
#endif