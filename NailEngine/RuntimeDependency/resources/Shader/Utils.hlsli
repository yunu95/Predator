#ifndef _UTILS_HLSLI_
#define _UTILS_HLSLI_

#include "Buffers.hlsli"

#include "Matrix.hlsli"
#include "Quaternion.hlsli"

float4x4 LerpTransformMatrix(float4x4 matrix1, float4x4 matrix2, float ratio)
{
    float3 pos1;
    float4 rotation1;
    float3 scale1;
    
    float3 pos2;
    float4 rotation2;
    float3 scale2;
    
    decompose(matrix1, pos1, rotation1, scale1);
    decompose(matrix2, pos2, rotation2, scale2);
    
    float3 finalPos = lerp(pos1, pos2, ratio);
    float4 finalRotation = q_slerp(rotation1, rotation2, ratio);
    float3 finalScale = lerp(scale1, scale2, ratio);

    return (compose(finalPos, finalRotation, finalScale));
}


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

//static const float PI = 3.141592;
static const float Epsilon = 0.00001;
static const float3 Fdielectric = 0.04;

///
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

uint querySpecularTextureLevels()
{
    uint width, height, levels;
    PrefilteredMap.GetDimensions(0, width, height, levels);
    return levels;
}
///



float CalcShadowFactor(SamplerComparisonState samShadow,
                       Texture2D shadowMap,
					   float4 shadowPosH)
{
	//// Complete projection by doing division by w.
    
    shadowPosH.xyz /= shadowPosH.w;
    
	//// Depth in NDC space.
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
    
    float2 tempuv = shadowPosH.xy;
    tempuv.y = -tempuv.y;
    tempuv = tempuv * 0.5 + 0.5;
    
	[unroll]
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow, tempuv + offsets[i], depth).r;
    }
    return percentLit /= 9.0f;
}

void CalculatePBRLight(int lightIndex, float3 normal, float3 pos, out float4 diffuse, out float4 ambient, out float4 specular, float3 albedo,float ao, float metalness, float roughness)
{
    // 나는 별도의 렌더타겟에 View Space에 대한 정보가 담겨 있어 연산은 View Space에서 이루어진다.
    diffuse = float4(0.f, 0.f, 0.f, 0.f);
    ambient = float4(0.f, 0.f, 0.f, 0.f);
    specular = float4(0.f, 0.f, 0.f, 0.f);
    
    // 뷰 디렉션
    float3 Lo = normalize(pos);
    Lo = -Lo;
    
    // 뷰와 노말의 각
    float cosLo = max(0.0, dot(normal, Lo));
    
    // 반사 벡터
    float3 Lr = 2.0 * cosLo * normal - Lo;
    
    // 프레넬
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    
    if (lights[lightIndex].lightType == 0)
    {
        float3 Li = normalize(mul(float4(-lights[lightIndex].direction.xyz, 0.f), VTM).xyz);
        float3 Lradiance = float3(1.f,1.f,1.f);
        
        // 하프 벡터
        float3 Lh = normalize(Li + Lo);
        
        float cosLi = max(0.0, dot(normal, Li));
        float cosLh = max(0.0, dot(normal, Lh));
        
        float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
        float D = ndfGGX(cosLh, max(0.01, roughness));
        float G = gaSchlickGGX(cosLi, cosLo, roughness);
       
        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
        
        float3 diffuseBRDF = kd * albedo / PI;
        
        float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
        
        float3 directionalLighting = 0;
        
        directionalLighting += (diffuseBRDF  + specularBRDF) * (Lradiance * cosLi);
        
        float3 ambientLighting = float3(0, 0, 0);
        {
            float3 irradiance = IrradianceMap.Sample(sam, normal).rgb;
            float3 F = fresnelSchlick(F0, cosLo);
            float3 kd = lerp(1.0 - F, 0.0, metalness);
            float3 diffuseIBL = kd * albedo * irradiance;
            uint specularTextureLevels = querySpecularTextureLevels(); //  텍스쳐의 최대 LOD 개수를 구한다.	
            float3 specularIrradiance = PrefilteredMap.SampleLevel(sam, Lr, roughness * specularTextureLevels).rgb;
            float2 specularBRDF = BrdfMap.Sample(spBRDF_Sampler, float2(cosLo, roughness)).rg;
            float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
            ambientLighting = (diffuseIBL + specularIBL)*ao;
        }
       
        ///
        float shadow = 1.f;
        
        if (length(cosLo))
        {
            matrix shadowVP = lightVP;
            
            // 여기서 pos는 View Space에서의 Position
            // VTMInv를 곱해서 월드 Space로
            float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
            
            // 현재 Position을 Light Space로
            float4 shadowClipPos = mul(worldPos, shadowVP);
            
            shadow = CalcShadowFactor(shadowSam, Temp2Map, shadowClipPos);
            //directionalLighting *= shadow;
            specular = (float4) 0;
        }
        ///
        
        diffuse.xyz += directionalLighting.xyz * lights[lightIndex].color.diffuse.xyz;
        //diffuse.w = 1.f;
        ambient.xyz = ambientLighting + lights[lightIndex].color.ambient.xyz;
        diffuse = float4(pow(float3(diffuse.xyz), 1.0 / 2.2), 1.0);
        
        //diffuse *= shadow;
    }
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
        
        //그림자

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
        
        //if (length(diffuse))
        //{
        //    matrix shadowVP = lightVP;
            
        //    // 여기서 pos는 View Space에서의 POsition
        //    // VTMInv를 곱해서 월드 Space로
        //    float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
            
        //    // 현재 Position을 Light Space로
        //    float4 shadowClipPos = mul(worldPos, shadowVP);
            
            
        //    //float3 uv = shadowClipPos.xyz / shadowClipPos.w;
        //    //uv.y = -uv.y;
        //    //uv = uv * 0.5 + 0.5;
        //    //float4 tempUV = float4(uv, shadowClipPos.w);
            
        //    float shadow = 1.f;
        //    shadow = CalcShadowFactor(shadowSam, Temp2Map, shadowClipPos);
            
        //    diffuse *= shadow;
        //    specular = (float4) 0;
        //}
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