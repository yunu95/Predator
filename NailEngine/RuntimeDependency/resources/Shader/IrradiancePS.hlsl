#include "Buffers.hlsli"
#include "Utils.hlsli"

const float2 invAtan = float2(0.1591f, 0.3183f);
float3 SampleSphericalMap(float3 vec)
{
    float2 uv = float2(atan2(vec.z, vec.x), asin(vec.y));
    uv *= invAtan;
    uv += 0.5f;
    return float3(uv, 0.0f);
    
    //float3 n = normalize(vec);
    //float azimuth = atan2(n.z, n.x);
    //float inclination = acos(n.y);
    //float u = azimuth / (2.0 * PI);
    //float v = inclination / PI;
    //return float3(u, v, 0.0f); // 3차원 좌표 반환
}

struct PixelIn
{
    float4 posH : SV_Position;
    float3 posL : POSITION;
};

float4 main(PixelIn input) : SV_TARGET
{
    float3 normal = normalize(input.posL);
    
    float3 irradiance = float3(0.f, 0.f, 0.f);
    
    float3 up = float3(0.f, 1.f, 0.f);
    float3 right = cross(up, normal);
    up = cross(normal, right);
    
    float sampleDelta = 0.025f;
    float nrSamples = 0.f;
 
    for (float phi = 0.f; phi < 2.f * PI; phi += sampleDelta)
    {
        for (float theta = 0.f; theta < 0.5f * PI; theta += sampleDelta)
        {
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            float3 sampleVec = (tangentSample.x * right) + (tangentSample.y * up) + (tangentSample.z * normal);

            float3 uvw = SampleSphericalMap(sampleVec);

            irradiance += CubeMap.Sample(sam, uvw).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    return float4(irradiance, 1.0f);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullFront
// DepthType : LessEqual
// BlendType : Default
// Sampler : Default