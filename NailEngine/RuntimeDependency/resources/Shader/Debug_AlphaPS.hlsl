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
};

float4 main(PixelIn input) : SV_TARGET
{
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
        color.rgb = pow(color.rgb, 2.2f);
    }
    
    //float4 albedo = AlbedoMap.Sample(sam, input.uv);
    //return input.color * materialColor;
    //return materialColor * color;
    return pow(materialColor * color, 1.f/2.2f);
    
    //return input.color * albedo;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : LessNoWrite
// BlendType : AlphaBlend
// Sampler : Default