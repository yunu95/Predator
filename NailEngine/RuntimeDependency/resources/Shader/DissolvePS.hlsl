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
};

// Temp0Map : Dissolve Texture
// float0 : Amount
// float1 : Edge Thickness

PS_OUT main(PixelIn input)
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PS_OUT output = (PS_OUT) 0;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (UseTexture(useAlbedo) == 1)
    {
        color = AlbedoMap.Sample(sam, input.uv);
    }
    
    color = pow(color, 2.2f);
    
    
    float4 materialTemp = materialColor;
    
    half dissolve_value = Temp0Map.Sample(sam, input.uv).r;
    clip(dissolve_value - temp_float0);
    //dissolve_value = 1 - dissolve_value;
    // 음수면 페이즈2 용 이미지 출력
    color = color + materialTemp * step(dissolve_value - temp_float0, temp_float1);
    output.color = color;
    
    if (UseTexture(useOpacity) == 1)
    {
        float tempOpa = OpacityMap.Sample(sam, input.uv).w;
        if (tempOpa == 0.f)
        {
            clip(-1);
        }
    }
    

    
    float3 viewNormal = input.normalV;
    if (UseTexture(useNormal) == 1)
    {
        // [0, 255] 범위에서 [0, 1]로 변환
        float3 tangentSpaceNormal = NormalMap.Sample(sam, input.uv).xyz;
        tangentSpaceNormal.y = 1 - tangentSpaceNormal.y;
        
        // [0, 1] 범위에서 [-1, 1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.tangentV, input.biNormalV, input.normalV };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }
    
    if (UseTexture(useARM) == 1)
    {
        float3 arm = ARMMap.Sample(sam, input.uv).xyz;
        
        output.arm.x = arm.x;
        output.arm.y = arm.y;
        output.arm.z = arm.z;
    }
    else
    {
        output.arm.x = 1.0f;
        output.arm.y = 1.0f;
        output.arm.z = 0.0f;
    }
    
    output.position = input.posV;
    if(castDecal == 1)
    {
        output.viewPosDecal = input.posV;
    }
    output.normal = float4(viewNormal.xyz, 1.f);
    
    /////output.color = color * materialColor;
   
    
    if (UseTexture(useEmission))
    {
        output.emissive = EmissionMap.Sample(sam, input.uv);
    }
    
    output.util = float4(lightMapUV[input.id].lightMapIndex, lightMapUV[input.id].isOutLine, AmbientExposure, DiffuseExposure);
    
    return output;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default
