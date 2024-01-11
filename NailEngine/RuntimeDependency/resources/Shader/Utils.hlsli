#ifndef _UTILS_HLSLI_
#define _UTILS_HLSLI_

#include "Buffers.hlsli"

bool UseTexture(uint useTexture)
{
    if (useTexture == 1)
    {
        return true;
    }
    return false;
}
static const float SMAP_SIZE = 2048.0f;
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2D shadowMap,
					   float4 shadowPosH)
{
	// Complete projection by doing division by w.
    shadowPosH.xyz /= shadowPosH.w;
	
	// Depth in NDC space.
    float depth = shadowPosH.z;

	// Texel size.
    const float dx = SMAP_DX;

    float percentLit = 0.0f;
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

	[unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow,
			shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit /= 9.0f;
}

void CalculateLight(int lightIndex, float3 normal, float3 pos, out float4 diffuse, out float4 ambient, out float4 specular)
{
    diffuse = float4(0.f, 0.f, 0.f, 0.f);
    ambient = float4(0.f, 0.f, 0.f, 0.f);
    specular = float4(0.f, 0.f, 0.f, 0.f);
    
    float3 viewDirection = normalize(pos);
    
    if (lights[lightIndex].lightType == 0)
    {
        float3 lightVec = normalize(mul(float4(lights[lightIndex].direction.xyz, 0.f), VTM).xyz);
        ambient = lights[lightIndex].color.ambient;
        
        float diffuseFactor = dot(-lightVec, normal);
        
        if (diffuseFactor > 0.f)
        {
            float3 v = normalize(lightVec + 2 * (saturate(dot(-lightVec, normal)) * normal));
            float specFactor = pow(max(dot(-viewDirection, v), 0.f), 16.f);
            
            diffuse += diffuseFactor * lights[lightIndex].color.diffuse;
            specular += specFactor * lights[lightIndex].color.specular;
        }
        
        // 그림자
        //if (length(diffuse) != 0)
        //{
        //    matrix shadowVP = lightVP;
        
        //    float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
        //    float4 shadowClipPos = mul(worldPos, shadowVP);
        //    float depth = shadowClipPos.z / shadowClipPos.w;
    
        //    float2 uv = shadowClipPos.xy / shadowClipPos.w;
        //    uv.y = -uv.y;
        //    uv = uv * 0.5 + 0.5;
 
        //    if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
        //    {
        //        float shadowDepth = Temp2Map.Sample(sam, uv).x;
        //        if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
        //        {
        //            diffuse *= 0.5f;
        //            specular = (float4) 0.f;
        //        }
        //    }
        //}
        
        if(length(diffuse))
        {
            matrix shadowVP = lightVP;
            
            float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
            float4 shadowClipPos = mul(worldPos, shadowVP);
            float depth = shadowClipPos.z / shadowClipPos.w;
            
            float2 uv = shadowClipPos.xy / shadowClipPos.w;
            uv.y = -uv.y;
            uv = uv * 0.5 + 0.5;
            
            if (0 < uv.x && uv.x < 1 && 0 < uv.y && uv.y < 1)
            {
                float shadowDepth = Temp2Map.Sample(sam, uv).x;
                if (shadowDepth > 0 && depth > shadowDepth + 0.00001f)
                {
                    float s0 = Temp2Map.Sample(shadowSam, uv).r;
                    float s1 = Temp2Map.Sample(shadowSam, uv + float2(SMAP_DX, 0)).r;
                    float s2 = Temp2Map.Sample(shadowSam, uv + float2(0, SMAP_DX)).r;
                    float s3 = Temp2Map.Sample(shadowSam, uv + float2(SMAP_DX, SMAP_DX)).r;
                    
                    float r0 = shadowDepth <= s0;
                    float r1 = shadowDepth <= s1;
                    float r2 = shadowDepth <= s2;
                    float r3 = shadowDepth <= s3;
                    
                    float2 texPos = SMAP_SIZE * uv;
                    
                    float2 t = frac(texPos);
                    
                    float temp = lerp(lerp(r0, r1, t.x),
                    lerp(r2, r3, t.x), t.y);
                    
                    diffuse *= temp;
                    specular = (float4) 0.f;
                }
            }
        }
    }
    else if (lights[lightIndex].lightType == 1)
    {
        float diffuseRatio = 0.f;
        float distanceRatio = 1.f;
        
        float3 lightPos = mul(lights[lightIndex].position, VTM).xyz;
        
        float3 lightVec = normalize(pos - lightPos);
        
        diffuseRatio = saturate(dot(-lightVec, normal));
        
        float d = distance(pos, lightPos);
        if (lights[lightIndex].range == 0.f)
        {
            distanceRatio = 0.f;
        }
        else
        {
            distanceRatio = saturate(1.f - pow(d / lights[lightIndex].range, 2));
        }
        
        ambient = lights[lightIndex].color.ambient * distanceRatio;
        diffuse = lights[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
       
        //float zero = 0.f;
        
        //if(diffuseFactor > zero)
        //{
        //    //float3 v = normalize(reflect(-lightVec, normal));
        //    //float specFactor = pow(max(dot(v, viewDirection), 0.f), 8.f);
            
        //    diffuse = diffuseFactor * lights[lightIndex].color.diffuse;
        //    //specular = specFactor * lights[lightIndex].color.specular;
        //}
        
        //float att = 1.f / pow(d, 2);
        
        //diffuse *= att;
        
        // specular *= att;
        
    }
    else if (lights[lightIndex].lightType == 2)
    {
        float3 lightVec = lights[lightIndex].position - pos;
        
        float d = length(lightVec);
        
        if (d > lights[lightIndex].range)
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