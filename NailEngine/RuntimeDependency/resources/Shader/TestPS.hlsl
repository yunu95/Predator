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

float4 main(PixelIn input) : SV_Target0
{
    //input.normalV = normalize(input.normalV);
    
    //float3 viewDirection = normalize(cameraPos.xyz - input.posW.xyz);
    
    //LightColor totalColor = (LightColor) 0.f;

    //float3 normal = input.normalV;
    
    //if(UseTexture(useNormal))
    //{
    //    float3 tNormal = NormalMap.Sample(sam, input.uv).xyz;
    //    tNormal = (tNormal - 0.5f) * 2.f;
    //    float3x3 matTBN = { input.tangentV, input.biNormalV, input.normalV };
    //    normal = normalize(mul(tNormal, matTBN));
    //}
    
    //for (int i = 0; i < lightCount; ++i)
    //{
    //    LightColor color;
    //    CalculateLight(i, normal, viewDirection, input.posW, color.diffuse, color.ambient, color.specular);
    //    totalColor.diffuse += color.diffuse;
    //    totalColor.ambient += color.ambient;
    //    totalColor.specular += color.specular;
    //}
    
    //if(UseTexture(useAlbedo))
    //{
    //    return AlbedoMap.Sample(sam,input.uv) * (totalColor.ambient + totalColor.diffuse) + totalColor.specular;
    //}
    //else
    //{
    //    return materialColor * (totalColor.ambient + totalColor.diffuse) + totalColor.specular;
    //}
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    float4 projPos = { 0, 0, 0, 0 };
    
    projPos = mul(input.posV, PTM);
    
    float depth = projPos.z / projPos.w;
    
    //output.depth = float4(objectID.x, 0, 0, 0);
    //return float4(depth, depth, depth,1.f);
    return depth;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : NoDepthTest
// BlendType : Default
