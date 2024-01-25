#include "Buffers.hlsli"
#include "Utils.hlsli"


float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; 
}

float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);
	
    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float NormalDistributionGGXTR(float3 normal, float3 halfwayVec, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = saturate(dot(normal, halfwayVec));
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1) + 1);
    denom = PI * denom * denom;

    return a2 / denom;
}

struct PixelIn
{
    float4 posH : SV_Position;
    float3 posL : POSITION;
};

float4 main(PixelIn input) : SV_TARGET
{
    float3 normalVec = normalize(input.posL);
    float3 R = normalVec;
    float3 viewDir = R;
    float roughness2 = 0.5f * 0.5f;
	
    float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;
	
    const uint NumSamples = 1024;
    for (uint i = 0; i < NumSamples; i++)
    {
        float2 Xi = Hammersley(i, NumSamples);
        float3 halfwayVec = ImportanceSampleGGX(Xi, normalVec, 0.5f);
        float3 lightDir = 2.0f * dot(viewDir, halfwayVec) * halfwayVec - viewDir;
        float NdotL = saturate(dot(normalVec, lightDir));
		
        if (NdotL > 0)
        {
            float D = NormalDistributionGGXTR(normalVec, halfwayVec, roughness2);
            float NdotH = max(dot(normalVec, halfwayVec), 0.0f);
            float HdotV = max(dot(halfwayVec, viewDir), 0.0f);
            float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f;

            float resolution = 512.0f;
            float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
            float saSample = 1.0f / (float(NumSamples) * pdf + 0.0001f);

            float mipLevel = 0.5f == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            PrefilteredColor += CubeMap.SampleLevel(sam, lightDir, 0).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    PrefilteredColor /= totalWeight;

    return float4(PrefilteredColor, 1.0f);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullFront
// DepthType : LessEqual
// BlendType : Default
// Sampler : Default