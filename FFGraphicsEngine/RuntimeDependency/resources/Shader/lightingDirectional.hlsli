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

struct PS_OUT
{
    float4 diffuse : SV_Target0;
    float4 specular : SV_Target1;
};

// Directional Light
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_tex_2 : Shadow RT
// g_tex_3 : Depth RT
// Mesh : Rectangle

static const float SMAP_SIZE = 4096.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

VS_OUT VS_DirLight(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;
    
    return output;
}

PS_OUT PS_DirLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT) 0;
    
    float3 viewPos = g_tex_0.Sample(g_sam_0, input.uv).xyz;
    if (viewPos.z <= 0.f)
    {
        clip(-1);
    }
    
    float3 viewNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
    
    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);
    
    // 그림자
    if (length(color.diffuse) != 0)
    {
        matrix shadowVP = g_matLightVP;
        
        float4 worldPos = mul(float4(viewPos.xyz, 1.f), g_matViewInv);
        float4 shadowClipPos = mul(worldPos, shadowVP);
        float depth = shadowClipPos.z / shadowClipPos.w;
    
        float2 uv = shadowClipPos.xy / shadowClipPos.w;
        uv.y = -uv.y;
        uv = uv * 0.5 + 0.5;
 
        if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
        {
            float shadowDepth = g_tex_2.Sample(g_sam_2, uv).x;
            if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
            {
                color.diffuse *= 0.5f;
                //color.ambient *= percentLit;
                color.specular = (float4) 0.f;
            }
        }
    }
    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;
    
    //// 외곽선 추출
    float2 texelSize = 1.0 / float2(1920, 1080); // 텍스처의 픽셀 크기 계산
    
    float mask[9] = { 0, -1, 0, -1, 4, -1, 0, -1, 0 }; // Laplacian Filter
    float coord[3] = { -1, 0, 1 };
   
    float4 backgroundColor = float4(0, 0, 0, 0); // 배경 색상 (검정색)
    
    float4 c0 = backgroundColor;
    
    for (int i = 0; i < 9; i++)
    {
        c0 += mask[i] * (g_tex_3.Sample(g_sam_0, input.uv + texelSize * float2(coord[i % 3], coord[i / 3])));
    }
    
    float4 edgeColor = g_tex_3.Sample(g_sam_0, input.uv); // 텍스처에서 샘플링한 색상을 외곽선 색상으로 사용
    
    float4 resultColor = lerp(backgroundColor, edgeColor, saturate(c0.r)); // 엣지 부분에만 edgeColor 블렌딩
    
    // 외곽선을 더 굵게 만들기 위해 주변 픽셀을 더해줌
    float outlineThickness = 2000.0;
    float4 outline = outlineThickness * (resultColor - edgeColor);
    
    output.specular = (resultColor + outline) * 2;
    
    return output;
}

#endif