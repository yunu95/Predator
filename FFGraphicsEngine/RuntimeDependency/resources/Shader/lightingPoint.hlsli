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

// [Point Light]
// g_int_0 : Light index
// g_tex_0 : Position RT
// g_tex_1 : Normal RT
// g_vec_0 : RenderTarget Resolution
// Mesh : Sphere

VS_OUT VS_PointLight(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;
    
    return output;
}

PS_OUT PS_PointLight(VS_OUT input)
{
    PS_OUT output = (PS_OUT)0;
    
    // input.pos = SV_Position = Screen ÁÂÇ¥
    float2 uv = float2(input.pos.x / g_vec2_0.x, input.pos.y / g_vec2_0.y);
    float3 viewPos = g_tex_0.Sample(g_sam_0, uv).xyz;
    if(viewPos.z <= 0.f)
    {
        clip(-1);
    }
    
    int lightIndex = g_int_0;
    float3 viewLightPos = mul(float4(g_light[lightIndex].position.xyz, 1.f), g_matView).xyz;
    float distance = length(viewPos - viewLightPos);
    if(distance > g_light[lightIndex].range)
    {
        clip(-1);
    }
    
    float3 viewNormal = g_tex_1.Sample(g_sam_0, uv).xyz;
    
    LightColor color = CalculateLightColor(g_int_0, viewNormal, viewPos);
    
    output.diffuse = color.diffuse + color.ambient;
    output.specular = color.specular;
    
    return output;
}
#endif