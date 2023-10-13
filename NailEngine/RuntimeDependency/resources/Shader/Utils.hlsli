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

LightColor CalculateLight(int lightIndex, float3 normal, float3 pos)
{
    LightColor color = (LightColor) 0.f;

    float3 viewLightDir = (float3) 0.f;

    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;

     viewLightDir = normalize(mul(float4(lights[lightIndex].direction.xyz, 0.f), VTM).xyz);
    diffuseRatio = saturate(dot(-viewLightDir, normal));
    
    float3 reflectionDir = normalize(viewLightDir + 2 * (saturate(dot(-viewLightDir, normal)) * normal));
    float3 eyeDir = normalize(pos);
    specularRatio = saturate(dot(-eyeDir, reflectionDir));
    specularRatio = pow(specularRatio, 2);

    color.diffuse = lights[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = lights[lightIndex].color.ambient * distanceRatio;
    color.specular = lights[lightIndex].color.specular * specularRatio * distanceRatio;

    //color.diffuse = float4(1.f, 0.f, 0.f, 0.f);
    //color.ambient = float4(1.f, 0.f, 0.f, 0.f);
    //color.specular = float4(1.f, 0.f, 0.f, 0.f);
    
    return color;
}

#endif