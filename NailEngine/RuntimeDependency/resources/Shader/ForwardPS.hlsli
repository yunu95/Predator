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

float4 main(PixelIn input)
{
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    if (UseTexture(useAlbedo) == 1)
    {
        color = AlbedoMap.Sample(sam, input.uv);
        //color.rgb = pow(color.rgb, 2.2f);
    }
  
    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
