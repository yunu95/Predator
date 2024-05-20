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
    float4 worldPos = mul(input.posV, VTMInv);
    float2 uv = float2(-worldPos.x, worldPos.z);
    // 알베도 블렌딩
    float4 color = AlbedoMap.Sample(sam, uv);
    
    float3 viewNormal = input.normalV;
    float3 tangentSpaceNormal = NormalMap.Sample(sam, uv).xyz;
        
    // [0, 1] 범위에서 [-1, 1]로 변환
    tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
    float3x3 matTBN = { input.tangentV, input.biNormalV, input.normalV };
    viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    
    if (UseTexture(useARM) == 1)
    {
        float3 arm = ARMMap.Sample(sam, uv);
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
        lightColor = UnityLightMap.Sample(sam, float3(input.lightUV, lightMapUV[input.id].lightMapIndex));
        //lightColor *= 0.18;
        //lightColor *= 0.6;
        //lightColor.rgb = pow(lightColor.rgb, 1.f / 2.2f);
        //lightColor.rgb = pow(lightColor.rgb,2.2f);
        float3 x = max(0, lightColor.xyz - 0.004);
        lightColor.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    
        output.color = color * lightColor;
        //output.color = color + lightColor;
    }
    else
    {
        color = pow(color, 2.2f);
        output.color = color;
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
