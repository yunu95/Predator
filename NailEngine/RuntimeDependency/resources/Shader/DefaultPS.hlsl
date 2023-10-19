#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 posW : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biNormal : BINORMAL;
};

float4 main(PixelIn input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    
    float3 viewDirection = normalize(cameraPos.xyz - input.posW.xyz);
    
    LightColor totalColor = (LightColor) 0.f;

    float3 normal = input.normal;
    
    if(UseTexture(useNormal))
    {
        float3 tNormal = NormalMap.Sample(sam, input.uv).xyz;
        tNormal = (tNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.tangent, input.biNormal, input.normal };
        normal = normalize(mul(tNormal, matTBN));
    }
    
    for (int i = 0; i < lightCount; ++i)
    {
        LightColor color;
        CalculateLight(i, normal, viewDirection, input.posW, color.diffuse, color.ambient, color.specular);
        totalColor.diffuse += color.diffuse;
        totalColor.ambient += color.ambient;
        totalColor.specular += color.specular;
    }
    
    if(UseTexture(useAlbedo))
    {
        return AlbedoMap.Sample(sam,input.uv) * (totalColor.ambient + totalColor.diffuse) + totalColor.specular;
    }
    else
    {
        return materialColor * (totalColor.ambient + totalColor.diffuse) + totalColor.specular;
    }
}

// ShaderInfo
// ShaderType : Solid
