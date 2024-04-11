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

// Temp0Map : Blend Albedo
// Temp1Map : Blend Normal
// Temp2Map : Blend ARM

PS_OUT main(PixelIn input)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PS_OUT output = (PS_OUT) 0;
    
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    if (UseTexture(useOpacity) == 1)
    {
        if (OpacityMap.Sample(sam, input.uv).w == 0.f)
        {
            clip(-1);
        }
    }
    
    if (UseTexture(useAlbedo) == 1)
    {
        color = AlbedoMap.Sample(sam, input.uv);
        float4 blendAlbedo = Temp0Map.Sample(sam, input.uv);
        
        color = lerp(color, blendAlbedo, input.color.r);
        
        color.rgb = pow(color.rgb, 2.2f);
    }
    
    float3 viewNormal = input.normalV;
    if (UseTexture(useNormal) == 1)
    {
        // [0, 255] 범위에서 [0, 1]로 변환
        float3 tangentSpaceNormal = NormalMap.Sample(sam, input.uv).xyz;
        float3 tangentSpaceNormal2 = Temp1Map.Sample(sam, input.uv).xyz;
        float3 tangentSpaceNormal3 = lerp(tangentSpaceNormal, tangentSpaceNormal2, input.color.r);
        
        // [0, 1] 범위에서 [-1, 1]로 변환
        tangentSpaceNormal3 = (tangentSpaceNormal3 - 0.5f) * 2.f;
        float3x3 matTBN = { input.tangentV, input.biNormalV, input.normalV };
        viewNormal = normalize(mul(tangentSpaceNormal3, matTBN));
    }
    
    if (UseTexture(useARM) == 1)
    {
        float3 arm = ARMMap.Sample(sam, input.uv);
        output.arm.x = arm.x;
        output.arm.y = arm.y;
        output.arm.z = arm.z;
    }
    else
    {
        output.arm.x = 1.f;
        output.arm.y = 1.f;
        output.arm.z = 0.f;
    }
    
    output.position = input.posV;
    output.normal = float4(viewNormal.xyz, 1.f);
    
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
    
    if (UseTexture(useEmission))
    {
        output.emissive = EmissionMap.Sample(sam, input.uv);
    }
    
   
    
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
