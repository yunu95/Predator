#ifndef _UI_HLSLI_
#define _UI_HLSLI_

#include "params.hlsli"
#include "utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    //float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    //output.color = input.color;
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    if(g_tex_on_0)
    {
        color = g_tex_0.Sample(g_sam_0, input.uv);
    }
    color = color * g_color;
    clip(color.a - 0.1f);
    
    return color;
}
#endif