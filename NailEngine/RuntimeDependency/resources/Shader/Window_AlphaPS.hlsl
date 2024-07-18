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

float4 main(PixelIn input) : SV_Target
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    
    if (UseTexture(useOpacity) == 1)
    {
        float tempOpa = OpacityMap.Sample(sam, input.uv).w;
        if (tempOpa == 0.f)
        {
            clip(-1);
        }
    }
    
    if (UseTexture(useAlbedo) == 1)
    {
        color = AlbedoMap.Sample(sam, input.uv);
        color.a = color.r;
        color = pow(color, 2.2f);
    }

    if ((lightMapUV[input.id].lightMapIndex != -1) && useLightMap)
    {
        float4 lightColor = float4(0, 0, 0, 1.f);
        lightColor = UnityLightMap.Sample(sam, float3(input.lightUV, lightMapUV[input.id].lightMapIndex));
        lightColor *= color;
        float3 x = max(0, lightColor.xyz - 0.004);
        lightColor.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    
        //output.color = color * lightColor;
        color = lightColor * materialColor;
    }
    else
    {
        //color = pow(color, 2.2f);
        color = color * materialColor;
    }
    
    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : LessNoWrite
// BlendType : AlphaBlend
// Sampler : Default
