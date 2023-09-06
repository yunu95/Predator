#ifndef _BOSS_FLAG_HLSLI_
#define _BOSS_FLAG_HLSLI_

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

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;
    
    return output;
}

// g_float_1 : 디졸브 할 것인지
// g_int_1 : 디졸브가 끝났는지
// g_tex_0 : 본래 이미지 (눈 없음)
// g_tex_1 : 디졸브용 이미지 (노이즈 맵)
// g_tex_2 : 페이즈2용 이미지 (눈 있음)
// g_float_0 : Amount

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.f, 0.f, 0.f, 1.f);
    
    // 디졸브 안 할 땐 본래 이미지 출력
    if(g_float_1 == 0.0f)
    {
        if (g_tex_on_0)
        {
            color = g_tex_0.Sample(g_sam_0, input.uv);
        }
    }
    else
    {
       // 디졸브가 끝나면 페이즈2용 이미지 출력
       if(g_int_1 == 1)
        {
            color = g_tex_2.Sample(g_sam_0, input.uv);
        }
        else
        {
            half dissolve_value = g_tex_1.Sample(g_sam_0, input.uv).r;
            dissolve_value = 1 - dissolve_value;
            // 음수면 페이즈2 용 이미지 출력
            if ((dissolve_value - g_float_0) < 0)
            {
                color = g_tex_2.Sample(g_sam_0, input.uv);
            }
            else
            {
                color = g_tex_0.Sample(g_sam_0, input.uv);
                color = color + g_color * step(dissolve_value - g_float_0, 0.01f);
            }

        }
    }
    
    clip(color.a - 0.1f);
    
    return color;
}
#endif