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

void CalculateLight(int lightIndex, float3 normal, float3 viewDirection, float4 pos, out float4 diffuse, out float4 ambient, out float4 specular)
{
    diffuse = float4(0.f, 0.f, 0.f, 0.f);
    ambient = float4(0.f, 0.f, 0.f, 0.f);
    specular = float4(0.f, 0.f, 0.f, 0.f);
    
    if(lights[lightIndex].lightType == 0)
    {
        float3 lightVec = -lights[lightIndex].direction;
        ambient = lights[lightIndex].color.ambient;
        
        float diffuseFactor = dot(lightVec, normal);
        
        if (diffuseFactor > 0.f)
        {
            float3 v = normalize(reflect(-lightVec, normal));
            float specFactor = pow(max(dot(v, viewDirection), 0.f), 36.f);
            
            diffuse = diffuseFactor * lights[lightIndex].color.diffuse;
            specular = specFactor * lights[lightIndex].color.specular;
        }
    }
    else if (lights[lightIndex].lightType == 1)
    {
        float3 lightVec = lights[lightIndex].position - pos;
        
        float d = length(lightVec);
        
        if(d > lights[lightIndex].range)
        {
            return;
        }
        
        lightVec /= d;
        
        ambient = lights[lightIndex].color.ambient;
        
        float diffuseFactor = dot(lightVec, normal);
        float zero = 0.f;
        
        if(diffuseFactor > zero)
        {
            float3 v = normalize(reflect(-lightVec, normal));
            float specFactor = pow(max(dot(v, viewDirection), 0.f), 8.f);
            
            diffuse = diffuseFactor * lights[lightIndex].color.diffuse;
            specular = specFactor * lights[lightIndex].color.specular;
        }
        
        float att = 1.f / pow(d, 2);
        
        diffuse *= att;
        specular *= att;
        
    }
    else if (lights[lightIndex].lightType == 2)
    {
        float3 lightVec = lights[lightIndex].position - pos;
        
        float d = length(lightVec);
        
        if(d > lights[lightIndex].range)
        {
            return;
        }
        
        lightVec /= d;
        
        ambient = lights[lightIndex].color.ambient;
        
        float diffuseFactor = dot(lightVec, normal);
        
        if (diffuseFactor > 0.f)
        {
            float3 v = normalize(reflect(-lightVec, normal));
            float specFactor = pow(max(dot(v, viewDirection), 0.f), 36.f); 
            
            diffuse = diffuseFactor * lights[lightIndex].color.diffuse;
            specular = specFactor * lights[lightIndex].color.specular;
        }
        
        float spot = pow(max(dot(-lightVec, lights[lightIndex].direction.xyz), 0.f), 16.f);
        
        float att = 1.f / pow(d, 2);
        
        ambient *= spot;
        diffuse *= att;
        specular *= att;
    }
}

#endif