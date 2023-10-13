#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 color : Color;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

float4 main(PixelIn input) : SV_TARGET
{
    float4 albedo = materialColor;
    float4 color = lights[0].color.ambient;
    float4 specular = lights[0].color.specular;
    float3 lightDir = -lights[0].direction;
    float lightIntensity = saturate(dot(input.normal, lightDir));
    
    if(lightIntensity > 0.f)
    {
        color += (lights[0].color.diffuse * lightIntensity);
        color = saturate(color);
        float3 reflection = normalize(2 * lightIntensity * input.normal - lightDir);
        specular = pow(saturate(dot(reflection, input.viewDirection)), 4.f);
    }
    
    color = color * albedo;
    color = saturate(color + specular);
    
    return color;
    //float4 albedo;
    //float4 lightColor;
    //float4 ambientColor;
    
    //float3 lightDir = -lights[0].direction;
    //float intensity = saturate(dot(input.normal, lightDir));
    
    //lightColor = saturate(lights[0].color.diffuse * intensity);
    //ambientColor = lights[0].color.ambient;
    
    //if (useAlbedo)
    //{
    //    albedo = AlbedoMap.Sample(sam, input.uv);
        
    //    if (intensity > 0.f)
    //    {
    //        ambientColor += (lightColor * intensity);
    //        ambientColor = saturate(ambientColor);
    //    }
        
    //    return albedo * ambientColor;
    //}
    //else
    //{
    //    return input.color * materialColor * lightColor;
    //}

    //LightColor totalColor = (LightColor) 0.f;

    ////for (int i = 0; i < lightCount; ++i)
    ////{
    ////    LightColor color = CalculateLight(i, input.normal, input.pos.xyz);
    ////    totalColor.diffuse += color.diffuse;
    ////    totalColor.ambient += color.ambient;
    ////    totalColor.specular += color.specular;
    ////}

    ////color1.xyz = (totalColor.diffuse.xyz * color1.xyz)
    ////    + totalColor.ambient.xyz * color1.xyz
    ////    + totalColor.specular.xyz;
}

// ShaderInfo
// ShaderType : Solid
