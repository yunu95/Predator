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
    float4 viewPosDecal : SV_Target6;
    float4 outlineInfo : SV_Target7;
};

// AlbedoMap : T_Dirt_BaseColor

// HeightMap : T_Grass_BaseColor

// Temp0Map : T_GrassBlend_BaseColor

PS_OUT main(PixelIn input)
{
    PS_OUT output = (PS_OUT) 0;
    float4 worldPos = mul(input.posV, VTMInv);
    float2 uv = float2(-worldPos.x, worldPos.z);
    uv *= 0.1f;
    // 알베도 블렌딩
    float4 color = AlbedoMap.Sample(sam, uv);
    float4 blendAlbedo = HeightMap.Sample(sam, uv);
    color = lerp(color, blendAlbedo, input.color.r);
    float4 blendAlbedo2 = Temp0Map.Sample(sam, uv);
    color = lerp(color, blendAlbedo2, input.color.g);
    color = pow(color, 2.2f);
    
    float3 viewNormal = input.normalV;
    //float3 finalTangentSpaceNormal;
    //float3 tangentSpaceNormal = NormalMap.Sample(sam, uv).xyz;
    //float3 tangentSpaceNormal2 = EmissionMap.Sample(sam, uv).xyz;
    //float3 tangentSpaceNormal3 = Temp1Map.Sample(sam, uv).xyz;
    //finalTangentSpaceNormal = lerp(tangentSpaceNormal, tangentSpaceNormal2, input.color.r);
    //finalTangentSpaceNormal = lerp(finalTangentSpaceNormal, tangentSpaceNormal3, input.color.g);
        
    //// [0, 1] 범위에서 [-1, 1]로 변환
    //finalTangentSpaceNormal = (finalTangentSpaceNormal - 0.5f) * 2.f;
    //float3x3 matTBN = { input.tangentV, input.biNormalV, input.normalV };
    //viewNormal = normalize(mul(finalTangentSpaceNormal, matTBN));
    
 
    output.arm.x = 1.f;
    output.arm.y = 1.f;
    output.arm.z = 0.f;
    
    output.position = input.posV;
    if(castDecal == 1)
    {
        output.viewPosDecal = input.posV;
    }
    output.normal = float4(viewNormal.xyz, 1.f);
    
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
    
    output.util = float4(lightMapUV[input.id].lightMapIndex, lightMapUV[input.id].isOutLine, AmbientExposure, DiffuseExposure);
    output.outlineInfo = float4(lightMapUV[input.id].outlineInfo.r, lightMapUV[input.id].outlineInfo.g, lightMapUV[input.id].outlineInfo.b, lightMapUV[input.id].outlineInfo.a);
    return output;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default
