#ifndef _POST_HLSLI_
#define _POST_HLSLI_

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

// [Post]
// g_tex_0 : EdgeDetect Target
// Mesh : Rectangle

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    
    output.pos = float4(input.pos * 2, 1.f);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 output = (float4)0;
    float4 color = float4(0, 0, 0, 0);
    
    color = g_tex_0.Sample(g_sam_0, input.uv);
    
    if ((color.x == 0.f) && (color.y == 0.f) && (color.z == 0.f))
    {
        clip(-1);
    }
    
    return color;
}
#endif