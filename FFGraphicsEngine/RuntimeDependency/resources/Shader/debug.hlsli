#ifndef _DEBUG_HLSLI_
#define _DEBUG_HLSLI_

#include "params.hlsli"

struct VS_TEX_IN
{
    float3 pos : POSITION;
};

struct VS_TEX_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VS_TEX_OUT VS_Main(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT) 0;
    
  
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.color = g_color;
    
    return output;
}

float4 PS_Main(VS_TEX_OUT input) : SV_Target
{
    float4 color = input.color;
    
    return color;
}

#endif