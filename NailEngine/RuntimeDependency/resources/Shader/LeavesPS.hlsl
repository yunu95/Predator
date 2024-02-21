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

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    float4 depth : SV_Target3;
    float4 arm : SV_Target4;
};


PS_OUT main(PixelIn input)
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
    
    PS_OUT output = (PS_OUT) 0;
    
    float4 color = float4(0.5f, 0.5f, 0.5f, 1.f);
    
    if (UseTexture(useAlbedo) == 1)
    {
        color = AlbedoMap.Sample(sam, input.uv);
        color.rgb = pow(color.rgb, 2.2f);
        clip(color.w - 1);
    }
    
    float3 viewNormal = input.normalV;
    if (UseTexture(useNormal) == 1)
    {
        // [0, 255] 범위에서 [0, 1]로 변환
        //float3 tangentSpaceNormal = pow(NormalMap.Sample(sam, input.uv).xyz, 1 / 2.2f);
        //float3 tangentSpaceNormal = pow(NormalMap.Sample(sam, input.uv).xyz, 2.2f);
        float3 tangentSpaceNormal = NormalMap.Sample(sam, input.uv).xyz;
        
        // [0, 1] 범위에서 [-1, 1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.tangentV, input.biNormalV, input.normalV };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }
    
    if (UseTexture(useOpacity) == 1)
    {
        color = OpacityMap.Sample(sam, input.uv);
        clip(color.w - 1);
    }
    
    output.arm.x = 1.f;
    output.arm.y = 0.5f;
    output.arm.z = 1.f;
    
    output.position = float4(input.posV.xyz, 1.f);
    output.normal = float4(viewNormal.xyz, 1.f);
    output.color = color * materialColor;
    
    float4 projPos = { 0, 0, 0, 0 };
    
    projPos = mul(input.posV, PTM);
    
    float depth = projPos.z / projPos.w;
    
    output.depth = float4(depth, depth, depth, depth);
    //output.depth = float4(objectID.x, 0, 0, 0);
    return output;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullFront
// DepthType : Less
// BlendType : Default
// Sampler : Default
