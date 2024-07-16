#ifndef _UTILS_HLSLI_
#define _UTILS_HLSLI_

#include "Buffers.hlsli"

#include "Matrix.hlsli"
#include "Quaternion.hlsli"

float CalCulateDepth(float3 pos, int index)
{
    float near = lights[index].nearPlane;
    float far = lights[index].farPlane;
    
    float c1 = far / (far - near);
    float c0 = -near * far / (far - near);
    float3 m = abs(pos).xyz;
    float major = max(m.x, max(m.y, m.z));
    float depth = (c1 * major + c0) / major;
    //return PointLightShadowMap.SampleCmpLevelZero(shadowSam, pos, depth);
    return depth;
}

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
static const float SMAP_SIZE = 4096.0f;
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
    float ndf = alphaSq / (PI * denom * denom);
    
    ndf = max(ndf, 0.0); // 최소값은 0으로 제한

    return ndf;
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
    
    if (tempuv.x < 0 || tempuv.x > 1 || tempuv.y < 0 || tempuv.y > 1)
    {
        return 1;
    }
    
    for (int i = 0; i < 9; ++i)
    {
        percentLit += shadowMap.SampleCmpLevelZero(samShadow, tempuv + offsets[i], depth).r;
    }
    
    return percentLit /= 9.0f;
}

//void CalculatePBRLight(int lightIndex, float3 normal, float3 pos, out float4 diffuse, out float4 ambient, out float4 specular, float3 albedo, float ao, float metalness, float roughness, float diffuseExposure, float ambientExposure, int useLightMap, out float shadowFactor)
//{
//    // 나는 별도의 렌더타겟에 View Space에 대한 정보가 담겨 있어 연산은 View Space에서 이루어진다.
//    diffuse = float4(0.f, 0.f, 0.f, 0.f);
//    ambient = float4(0.f, 0.f, 0.f, 0.f);
//    specular = float4(0.f, 0.f, 0.f, 0.f);
    
//    // 뷰 디렉션
//    float3 Lo = normalize(pos);
//    Lo = -Lo;
    
//    // 뷰와 노말의 각
//    float cosLo = max(0.0, dot(normal, Lo));
    
//    // 반사 벡터
//    float3 Lr = 2.0 * cosLo * normal - Lo;
    
//    // 프레넬
//    float3 F0 = lerp(Fdielectric, albedo, metalness);
    
//    if (lights[lightIndex].lightType == 0)
//    {
//        float3 Li = normalize(mul(float4(-lights[lightIndex].direction.xyz, 0.f), VTM).xyz);
//        float3 Lradiance = float3(1, 1, 1);
        
//        // 하프 벡터
//        float3 Lh = normalize(Li + Lo);
        
//        float cosLi = max(0.0, dot(normal, Li));
//        float cosLh = max(0.0, dot(normal, Lh));
        
//        float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
//        float D = ndfGGX(cosLh, max(0.01, roughness));
//        float G = gaSchlickGGX(cosLi, cosLo, roughness);
       
//        float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
        
//        float3 diffuseBRDF = kd * albedo / PI;
        
//        float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
        
//        float3 directionalLighting = 0;
        
//        directionalLighting += (diffuseBRDF + specularBRDF) * (Lradiance * cosLi);
        
//        float3 ambientLighting = float3(0, 0, 0);
//        // 나중에 IBL쓰면 아래 코드 사용 만일 안쓴다면 기본 라이트의 엠비언트사용하게 해야 함
        
       
//        ///
//        if (length(cosLo))
//        //if (length(cosLo) && (useLightMap == -1))
//        {
//            matrix shadowVP = lightVP;
            
//            // 여기서 pos는 View Space에서의 Position
//            // VTMInv를 곱해서 월드 Space로
//            float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
            
//            // 현재 Position을 Light Space로
//            float4 shadowClipPos = mul(worldPos, shadowVP);
            
//            shadowFactor = CalcShadowFactor(shadowSam, Temp2Map, shadowClipPos);
            
//            specular = (float4) 0;
//        }
//        ///
        
//        //float expo = 0.5f;
//        directionalLighting *= diffuseExposure;
//        float3 x = max(0, directionalLighting.xyz - 0.004);
//        directionalLighting.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
        
//        diffuse.xyz += directionalLighting.xyz * lights[lightIndex].color.diffuse.xyz * lights[lightIndex].intensity;
//        //diffuse.xyz += directionalLighting.xyz * lights[lightIndex].color.diffuse.xyz;
//        diffuse.w = 1.f;
        
        
//        if (useIBL == 1)
//        {
//            float3 irradiance = IrradianceMap.Sample(sam, normal).rgb;
//            float3 F = fresnelSchlick(F0, cosLo);
//            float3 kd = lerp(1.0 - F, 0.0, metalness);
//            float3 diffuseIBL = kd * albedo * irradiance;
//            uint specularTextureLevels = querySpecularTextureLevels(); //  텍스쳐의 최대 LOD 개수를 구한다.	
//            float3 specularIrradiance = PrefilteredMap.SampleLevel(sam, Lr, roughness * specularTextureLevels).rgb;
//            float2 specularBRDF = BrdfMap.Sample(spBRDF_Sampler, float2(cosLo, roughness)).rg;
//            float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
//            ambientLighting = (diffuseIBL + specularIBL) * ao;
            
//            ambientLighting *= ambientExposure;
//            ambientLighting = ambientLighting / (1 + ambientLighting);
//            ambientLighting = pow(ambientLighting, 1 / 2.2);
            
//            ambient.xyz = ambientLighting;
//        }
//        else
//        {
//            ambient.xyz = lights[lightIndex].color.ambient;
//        }
//    }
//    else if (lights[lightIndex].lightType == 1)
//    {
//        // Point light의 위치
//        float3 lightPos = mul(lights[lightIndex].position, VTM).xyz;
        
//        // 표면과 Point light 사이의 거리 계산
//        float distance = length(lightPos - pos);
        
//        float distanceRatio = 1.f;
        
//        // 조명 반경 내에 있는 표면의 영향 계산
//        if (distance <= lights[lightIndex].range)
//        {
//            distanceRatio = saturate(1.0f - pow(distance / lights[lightIndex].range, 2.0f));
            
//            // Point light의 빛 세기
//            float3 Lradiance = float3(1.f, 1.f, 1.f);
            
//            // 빛의 방향 벡터
//            float3 Li = normalize(pos - lightPos);
//            Li = -Li;
            
//            // 하프 벡터
//            float3 Lh = normalize(Li + Lo);
            
//            float cosLi = max(0.0, dot(normal, Li));
//            float cosLh = max(0.0, dot(normal, Lh));
            
//            float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
//            float D = ndfGGX(cosLh, max(0.01, roughness));
//            float G = gaSchlickGGX(cosLi, cosLo, roughness);
           
//            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
            
//            float3 diffuseBRDF = kd * albedo / PI;
            
//            float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
            
//            float3 pointLighting = (diffuseBRDF + specularBRDF) * (Lradiance * cosLi);
            
            
//            pointLighting *= diffuseExposure;
//            float3 x = max(0, pointLighting.xyz - 0.004);
//            pointLighting.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
//            diffuse.xyz += pointLighting.xyz * lights[lightIndex].color.diffuse.xyz * distanceRatio * lights[lightIndex].intensity;
//            //diffuse.xyz += pointLighting.xyz * lights[lightIndex].color.diffuse.xyz * distanceRatio;
//            diffuse.w = 1.f;
            
//            float3 ambientLighting = float3(0, 0, 0);
//            if (useIBL == 1)
//            {
//                float3 irradiance = IrradianceMap.Sample(sam, normal).rgb;
//                float3 F = fresnelSchlick(F0, cosLo);
//                float3 kd = lerp(1.0 - F, 0.0, metalness);
//                float3 diffuseIBL = kd * albedo * irradiance;
//                uint specularTextureLevels = querySpecularTextureLevels(); //  텍스쳐의 최대 LOD 개수를 구한다.	
//                float3 specularIrradiance = PrefilteredMap.SampleLevel(sam, Lr, roughness * specularTextureLevels).rgb;
//                float2 specularBRDF = BrdfMap.Sample(spBRDF_Sampler, float2(cosLo, roughness)).rg;
//                float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
//                ambientLighting = (diffuseIBL + specularIBL) * ao;
                
//                ambientLighting *= ambientExposure;
//                ambientLighting = ambientLighting / (1 + ambientLighting);
//                ambientLighting = pow(ambientLighting, 1 / 2.2);
            
//                //ambient.xyz = ambientLighting;
//            }
//            else
//            {
//                ambient.xyz = lights[lightIndex].color.ambient;
//            }
            
//            // Shadow 연산하는 코드 넣기
//            float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
//            float curDepth = CalCulateDepth((worldPos.xyz - lights[lightIndex].position.xyz), lightIndex);
        
//            if (isShadowCast && (useLightMap == -1))
//            {
//                shadowFactor = PointLightShadowMap.Sample(sam, float4(normalize(worldPos.xyz - lights[lightIndex].position.xyz), plIndex)).r;
                
//                if (shadowFactor < curDepth - 0.0001f)
//                {
//                    shadowFactor = 0.0f;
//                }
//                else
//                {
//                    shadowFactor = 1.0f;
//                }
//            }
//        }
//    }
//}

void CalculateDirectionalPBR(int lightIndex, float3 normal, float3 pos, out float4 diffuse, out float4 ambient, out float4 specular, float3 albedo, float ao, float metalness, float roughness, float diffuseExposure, float ambientExposure, int useLightMap, out float shadowFactor)
{
    shadowFactor = 1.f;
    // 나는 별도의 렌더타겟에 View Space에 대한 정보가 담겨 있어 연산은 View Space에서 이루어진다.
    diffuse = float4(0.f, 0.f, 0.f, 0.f);
    ambient = float4(0.f, 0.f, 0.f, 0.f);
    specular = float4(0.f, 0.f, 0.f, 0.f);
    
    float4 specularNoAlbedo = float4(0.f, 0.f, 0.f, 0.f);
    
    
    // 뷰 디렉션
    float3 Lo = normalize(pos);
    Lo = -Lo;
    
    // 뷰와 노말의 각
    float cosLo = max(0.0, dot(normal, Lo));
    
    // 반사 벡터
    float3 Lr = 2.0 * cosLo * normal - Lo;
    
    // 프레넬
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    //float3 F0_no_albedo = float3(0.04, 0.04, 0.04); // 고정된 값으로 프레넬 반사 설정
    //float3 F0_no_albedo = lerp(float3(0.04, 0.04, 0.04), float3(1.0, 1.0, 1.0), metalness); // 메탈릭 반영
    float3 F0_no_albedo = lerp(float3(0.04, 0.04, 0.04), albedo, metalness); // 메탈릭 반영
    
    float3 Li = normalize(mul(float4(-lights[lightIndex].direction.xyz, 0.f), VTM).xyz);
    float3 Lradiance = float3(1, 1, 1);
        
    // 하프 벡터
    float3 Lh = normalize(Li + Lo);
        
    float cosLi = max(0.0, dot(normal, Li));
    float cosLh = max(0.0, dot(normal, Lh));
        
    float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
    float3 F_no_albedo = fresnelSchlick(F0_no_albedo, max(0.0, dot(Lh, Lo)));
    float D = ndfGGX(cosLh, max(0.01, roughness));
    float G = gaSchlickGGX(cosLi, cosLo, roughness);
    
    // 기존 알베도 값을 반영한 kd 계산
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
    
    float3 diffuseBRDF = kd * albedo / PI;
    
    // 알베도 값을 반영한 스펙큘러 BRDF 계산
    float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

    // 알베도 값을 반영하지 않은 스펙큘러 BRDF 계산
    float3 specularBRDFNoAlbedo = (F_no_albedo * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
    
    float3 directionalLighting = 0;
    
    directionalLighting += (diffuseBRDF + specularBRDF) * (Lradiance * cosLi);
    
    float3 ambientLighting = float3(0, 0, 0);
    
    if (length(cosLo))
    {
        matrix shadowVP = lightVP;
        float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
        float4 shadowClipPos = mul(worldPos, shadowVP);
        shadowFactor = CalcShadowFactor(shadowSam, Temp2Map, shadowClipPos);
    }
    
    specularNoAlbedo = float4(specularBRDFNoAlbedo * (Lradiance * cosLi), 1.f);
    specularNoAlbedo *= diffuseExposure;
    float3 x1 = max(0, specularNoAlbedo.xyz - 0.004);
    specularNoAlbedo.xyz = (x1 * (6.2 * x1 + 0.5)) / (x1 * (6.2 * x1 + 1.7) + 0.06);
    
    directionalLighting *= diffuseExposure;
    //directionalLighting *= 1.f;
    float3 x = max(0, directionalLighting.xyz - 0.004);
    directionalLighting.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    
    diffuse.xyz += directionalLighting.xyz * lights[lightIndex].color.diffuse.xyz * lights[lightIndex].intensity;
    diffuse.w = 1.f;
    
    if (useIBL == 1)
    {
        float3 irradiance = IrradianceMap.Sample(sam, normal).rgb;
        float3 F = fresnelSchlick(F0, cosLo);
        float3 F_no_albedo = fresnelSchlick(F0_no_albedo, cosLo);
        float3 kd = lerp(1.0 - F, 0.0, metalness);
        float3 diffuseIBL = kd * albedo * irradiance;
        uint specularTextureLevels = querySpecularTextureLevels();
        float3 specularIrradiance = PrefilteredMap.SampleLevel(sam, Lr, roughness * specularTextureLevels).rgb;
        float2 specularBRDF = BrdfMap.Sample(spBRDF_Sampler, float2(cosLo, roughness)).rg;
        
        // IBL에서 알베도 값을 반영한 스펙큘러 계산
        float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;

        // IBL에서 알베도 값을 반영하지 않은 스펙큘러 계산
        float3 specularIBLNoAlbedo = (F_no_albedo * specularBRDF.x + specularBRDF.y) * specularIrradiance;
        
        ambientLighting = (diffuseIBL + specularIBL) * ao;
        
        ambientLighting *= ambientExposure;
        //ambientLighting *= 1.f;
        ambientLighting = ambientLighting / (1 + ambientLighting);
        ambientLighting = pow(ambientLighting, 1 / 2.2);
        
        ambient.xyz = ambientLighting;
        
        float4 tempSpeNoAlbedo = float4(specularIBLNoAlbedo * ao, 1.f);
        tempSpeNoAlbedo *= ambientExposure;
        tempSpeNoAlbedo = tempSpeNoAlbedo / (1 + tempSpeNoAlbedo);
        
        specularNoAlbedo += tempSpeNoAlbedo;
    }
    else
    {
        ambient.xyz = lights[lightIndex].color.ambient;
    }

    specular = specularNoAlbedo;
}

void CalculatePointPBR(int lightIndex, float3 normal, float3 pos, out float4 diffuse, out float4 ambient, out float4 specular, float3 albedo, float ao, float metalness, float roughness, float diffuseExposure, float ambientExposure, int useLightMap, out float shadowFactor)
{
    shadowFactor = 1.f;
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
    

        // Point light의 위치
    float3 lightPos = mul(lights[lightIndex].position, VTM).xyz;
        
        // 표면과 Point light 사이의 거리 계산
    float distance = length(lightPos - pos);
        
    float distanceRatio = 1.f;
        
        // 조명 반경 내에 있는 표면의 영향 계산
    if (distance <= lights[lightIndex].range)
    {
        distanceRatio = saturate(1.0f - pow(distance / lights[lightIndex].range, 2.0f));
            
            // Point light의 빛 세기
        float3 Lradiance = float3(1.f, 1.f, 1.f);
            
            // 빛의 방향 벡터
        float3 Li = normalize(pos - lightPos);
        Li = -Li;
            
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
            
        float3 pointLighting = (diffuseBRDF + specularBRDF) * (Lradiance * cosLi);
            
            
        pointLighting *= diffuseExposure;
        float3 x = max(0, pointLighting.xyz - 0.004);
        pointLighting.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
        diffuse.xyz += pointLighting.xyz * lights[lightIndex].color.diffuse.xyz * distanceRatio * lights[lightIndex].intensity;
            //diffuse.xyz += pointLighting.xyz * lights[lightIndex].color.diffuse.xyz * distanceRatio;
        diffuse.w = 1.f;
            
        float3 ambientLighting = float3(0, 0, 0);
        if (useIBL == 1)
        {
            float3 irradiance = IrradianceMap.Sample(sam, normal).rgb;
            float3 F = fresnelSchlick(F0, cosLo);
            float3 kd = lerp(1.0 - F, 0.0, metalness);
            float3 diffuseIBL = kd * albedo * irradiance;
            uint specularTextureLevels = querySpecularTextureLevels(); //  텍스쳐의 최대 LOD 개수를 구한다.	
            float3 specularIrradiance = PrefilteredMap.SampleLevel(sam, Lr, roughness * specularTextureLevels).rgb;
            float2 specularBRDF = BrdfMap.Sample(spBRDF_Sampler, float2(cosLo, roughness)).rg;
            float3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
            ambientLighting = (diffuseIBL + specularIBL) * ao;
                
            ambientLighting *= ambientExposure;
            ambientLighting = ambientLighting / (1 + ambientLighting);
            ambientLighting = pow(ambientLighting, 1 / 2.2);
            
                //ambient.xyz = ambientLighting;
        }
        else
        {
            ambient.xyz = lights[lightIndex].color.ambient;
        }
            
        // Shadow 연산하는 코드 넣기
        float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
        float curDepth = CalCulateDepth((worldPos.xyz - lights[lightIndex].position.xyz), lightIndex);
        
        if (isShadowCast)
        {
            shadowFactor = PointLightShadowMap.Sample(sam, float4(normalize(worldPos.xyz - lights[lightIndex].position.xyz), plIndex)).r;
                
            if (shadowFactor < curDepth - 0.0001f)
            {
                // 그림자가 지는 곳
                shadowFactor = 0.0f;
            }
            else
            {
                shadowFactor = 1.0f;
            }
        }
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
            //distanceRatio = saturate(1.0f - pow(d / lights[lightIndex].range, 1.f/2.0f));
            distanceRatio = saturate(1.0f - pow(d / lights[lightIndex].range, 2.0f));
        }
        
        
        ambient = lights[lightIndex].color.ambient * distanceRatio;
        diffuse = lights[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
        

        // Shadow 연산하는 코드 넣기
        float4 worldPos = mul(float4(pos.xyz, 1.f), VTMInv);
        float curDepth = CalCulateDepth((worldPos.xyz - lights[lightIndex].position.xyz), lightIndex);
        
        float shadow = PointLightShadowMap.Sample(sam, float4(normalize(worldPos.xyz - lights[lightIndex].position.xyz), plIndex)).r;
        
        if (shadow < curDepth - 0.01f)
        {
            shadow = 0.0f;
        }
        else
        {
            shadow = 1.0f;
        }
        diffuse *= shadow;
        
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

float3 BlendHardLight(float3 backdrop, float3 source)
{
    float maxRGB = max(source.r, source.g);
    maxRGB = max(maxRGB, source.b);

    float blend = smoothstep(0.2, 0.8, maxRGB);

    float3 multiply = backdrop * (2.0 * source);
    float3 screen = backdrop - (1.0 - 2.0 * source) - (backdrop * (1.0 - 2.0 * source));

    return lerp(multiply, screen, blend);
}

#endif