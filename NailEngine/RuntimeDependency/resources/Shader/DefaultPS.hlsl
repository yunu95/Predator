#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 color : Color;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(PixelIn input) : SV_TARGET
{
    float4 albedo;
    float4 color;
    if(useAlbedo)
    {
        albedo = AlbedoMap.Sample(sam, input.uv);
        
        float3 lightDir = -lights[0].direction;
        float intensity = saturate(dot(input.normal, lightDir));
    
        color = saturate(lights[0].color.diffuse * intensity);
    
        color = color * albedo;

        return color;
    }
    else
    {
        float4 lightColor;
        float3 lightDir = -lights[0].direction;
        float intensity = saturate(dot(input.normal, lightDir));
    
        lightColor = saturate(lights[0].color.diffuse * intensity);
    
        color = input.color * materialColor * lightColor;

        return color;
    }

    LightColor totalColor = (LightColor) 0.f;

    //for (int i = 0; i < lightCount; ++i)
    //{
    //    LightColor color = CalculateLight(i, input.normal, input.pos.xyz);
    //    totalColor.diffuse += color.diffuse;
    //    totalColor.ambient += color.ambient;
    //    totalColor.specular += color.specular;
    //}

    //color1.xyz = (totalColor.diffuse.xyz * color1.xyz)
    //    + totalColor.ambient.xyz * color1.xyz
    //    + totalColor.specular.xyz;
    
    float3 lightDir = -lights[0].direction;
    float intensity = saturate(dot(input.normal, lightDir));
    
    color = saturate(lights[0].color.diffuse * intensity);
    
    color = color * albedo;

    return color;
}

// ShaderInfo
// ShaderType : Solid
