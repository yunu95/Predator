#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 posH : SV_Position;
    float4 posV : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 tangentV : TANGENT;
    float3 biNormalV : BINORMAL;
    float2 lightUV : TEXCOORD1;
    uint id : ID;
};

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    float4 util : SV_Target3;
    float4 arm : SV_Target4;
    float4 emissive : SV_Target5;
};


PS_OUT main(PixelIn input)
{
    PS_OUT output = (PS_OUT) 0;
    
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.f);

    clip(OpacityMap.Sample(sam, input.uv).w - 1);
    
    color = AlbedoMap.Sample(sam, input.uv);
    
    
    if ((lightMapUV[input.id].lightMapIndex != -1) && useLightMap)
    {
        float4 lightColor = float4(0, 0, 0, 1.f);
        lightColor = UnityLightMap.Sample(sam, input.lightUV);
        lightColor *= 0.6;
        lightColor.rgb = pow(lightColor.rgb, 1.f / 2.2f);
    
        output.color = color * lightColor;
    }
    else
    {
        color = pow(color, 2.2f);
        output.color = color;
    }
    
    float3 viewNormal = input.normalV;
    
    output.arm.xyz = float3(1.f, 1.f, 0.f);
    
    output.position = float4(input.posV.xyz, 1.f);
    output.normal = float4(viewNormal.xyz, 1.f);
    output.util = float4(lightMapUV[input.id].lightMapIndex, DiffuseExposure, AmbientExposure, 1.f);
    return output;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default
