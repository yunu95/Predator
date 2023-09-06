#ifndef _TEXTURE_ANIM_HLSLI_
#define _TEXTURE_ANIM_HLSLI_

#include "params.hlsli"
#include "utils.hlsli"

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
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
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}

// g_int_0 : 다 재생 되었는지
// g_vec2_0 : direction
/// g_float_0 : speed
/// g_float_1 : Time 
float4 PS_Main(VS_OUT input) : SV_Target
{
    //float speed = g_float_0;
    //float time = g_float_1;
    
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    if (g_int_0 == 1)
    {
        if (g_tex_on_0)
        {
            input.uv = float2(g_vec2_0.x, g_vec2_0.y);
            color = g_tex_0.Sample(g_sam_0, input.uv);
            color.a = color.a * input.color.a;
        }
    }
    else
    {
        if (g_tex_on_0)
        {
            input.uv = input.uv + float2(g_vec2_0.x, g_vec2_0.y);
            color = g_tex_0.Sample(g_sam_0, input.uv);
            color.a = color.a * input.color.a;
        }
    }
    
    clip(color.a - 0.1f);
    
    return color;
}
#endif