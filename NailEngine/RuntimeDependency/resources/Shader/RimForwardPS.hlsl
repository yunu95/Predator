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
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PS_OUT output = (PS_OUT) 0;
    
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (UseTexture(useOpacity) == 1)
    {
        float tempOpa = OpacityMap.Sample(sam, input.uv).w;
        if (tempOpa == 0.f)
        {
            clip(-1);
        }
    }
  
    
    //float rim = saturate(dot(input.normalV, float3(0, 0, 1)));
    //rim = pow(1 - rim, 10);
    output.emissive.xyz = float3(1, 1, 1);
    output.emissive.a = 0.9;
    
    
    return output;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : AlphaBlend
// Sampler : Default
