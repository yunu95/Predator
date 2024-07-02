#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 posH : SV_Position;
    float4 posV : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 normalW : NORMAL1;
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

    clip(OpacityMap.Sample(sam, input.uv).w - 1.f);
    
    color = AlbedoMap.Sample(sam, input.uv);
    float dotValue = dot(input.normalW, float3(0, 1, 0));
    //color.x = lerp(color.x, (0.15), dotValue * 0.8);
    //color.y = lerp(color.y, (0.26), dotValue * 0.8);
    //color.z = lerp(color.z, (0.06), dotValue * 0.8);
    color.x = lerp(color.x, (0.8), dotValue * 0.3);
    color.y = lerp(color.y, (0.8), dotValue * 0.3);
    color.z = lerp(color.z, (0), dotValue * 0.3);
    

    //color = lerp(color, (1, 0, 0), dot(input.normalW, (0, 1, 0)) * 1);
    color = pow(color, 2.2f);
    
    if ((lightMapUV[input.id].lightMapIndex != -1) && useLightMap)
    {
        float4 lightColor = float4(0, 0, 0, 1.f);
        lightColor = UnityLightMap.Sample(sam, float3(input.lightUV, lightMapUV[input.id].lightMapIndex));
        lightColor *= color;
        float3 x = max(0, lightColor.xyz - 0.004);
        lightColor.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    
        //output.color = color * lightColor;
        output.color = lightColor;
    }
    else
    {
        //color = pow(color, 2.2f);
        output.color = color;
    }
    
    
    
    ////float4 lightColor = float4(0, 0, 0, 1.f);
    ////lightColor = UnityLightMap.Sample(sam, float3(input.lightUV, lightMapUV[input.id].lightMapIndex));
    ////lightColor *= 0.6;
    ////lightColor.rgb = pow(lightColor.rgb, 1.f / 2.2f);
    ////lightColor.rgb = pow(lightColor.rgb, 2.2f);
    
    ////output.color = color * lightColor;
    
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
