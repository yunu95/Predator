#ifndef _SKYBOX_HLSLI_
#define _SKYBOX_HLSLI_

#include "params.hlsli"

struct VS_IN
{
    float3 localPos : POSITION;
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

   // Translation은 하지 않고 Rotation만 적용한다.
    float4 viewPos = mul(float4(input.localPos, 0.0f),g_matView);
    float4 clipSpacePos = mul(viewPos, g_matProjection);
    
    // w/w=1이기 때문에 항상 깊이가 1로 유지된다.
    output.pos = clipSpacePos.xyww;
    output.uv = input.uv;
        
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(0.f,0.f,0.f,0.f);
    if(g_tex_on_0)
    {
        color = g_tex_0.Sample(g_sam_0, input.uv);
    }
    
    return color;
}

#endif