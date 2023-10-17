#ifndef _UTILS_HLSLI_
#define _UTILS_HLSLI_

#include "Buffers.hlsli"

bool UseTexture(uint useTexture)
{
    if(useTexture == 1)
    {
        return true;
    }
    return false;
}

LightColor CalculateLight(int lightIndex, float3 normal, float3 viewDirection, float4 pos)
{
    LightColor color = (LightColor) 0.f;

    float3 viewLightDir = (float3) 0.f;

    float diffuseFactor = 0.f;
    float specularFactor = 0.f;
    float distanceRatio = 1.f;
    
    if(lights[lightIndex].lightType == 0)
    {
        diffuseFactor = saturate(dot(-lights[lightIndex].direction, float4(normal, 0.f)));
    
        if (diffuseFactor > 0.f)
        {
            float3 reflection = normalize(reflect(lights[lightIndex].direction, float4(normal, 0.f)));
            specularFactor = pow(max(dot(reflection, viewDirection), 0.f), 32.f);
        }
    }
    else if (lights[lightIndex].lightType == 1)
    {
        float3 lightVec = lights[lightIndex].position - pos;
        
        float d = length(lightVec);
        diffuseFactor = saturate(dot(-lights[lightIndex].direction, float4(normal, 0.f)));
        float3 reflection = normalize(reflect(lights[lightIndex].direction, float4(normal, 0.f)));
        specularFactor = pow(max(dot(reflection, viewDirection), 0.f), 32.f);
        if (d > lights[lightIndex].range)
        {
            diffuseFactor = 0;
            specularFactor = 0;
        }
        lightVec /= d;
        
        distanceRatio = 1.f / pow(d, 2);
    }
   

    color.diffuse = lights[lightIndex].color.diffuse * diffuseFactor * distanceRatio;
    color.ambient = lights[lightIndex].color.ambient * distanceRatio;
    color.specular = lights[lightIndex].color.specular * specularFactor * distanceRatio;
    
    return color;
}

#endif