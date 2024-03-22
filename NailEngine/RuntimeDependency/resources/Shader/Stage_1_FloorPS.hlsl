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
    float4 emissive : SV_Target5;
};

// AlbedoMap : T_Dirt_BaseColor
// NormalMap : T_Dirt_Normal
// ARMMap : T_Dirt_ARM

// HeightMap : T_Grass_BaseColor
// EmissionMap : T_Grass_Normal
// OpacityMap : T_Grass_ARM

// Temp0Map : T_GrassBlend_BaseColor
// Temp1Map : T_GrassBlend_Normal
// Temp2Map : T_GrassBlend_ARM

// Temp3Map : T_Tile_BaseColor
// Temp4Map : T_Tile_Normal
// Temp5Map : T_Tile_ARM

// Temp6Map : T_TileBlend_BaseColor
// Temp7Map : T_TileBlend_Normal
// Temp8Map : T_TileBlend_ARM

PS_OUT main(PixelIn input)
{
    PS_OUT output = (PS_OUT) 0;
    
    // 알베도 블렌딩
    float4 color = AlbedoMap.Sample(sam, input.uv);
    float4 blendAlbedo = HeightMap.Sample(sam, input.uv);
    color = lerp(color, blendAlbedo, input.color.r);
    float4 blendAlbedo2 = Temp0Map.Sample(sam, input.uv);
    color = lerp(color, blendAlbedo2, input.color.g);
    float4 blendAlbedo3 = Temp3Map.Sample(sam, input.uv);
    color = lerp(color, blendAlbedo3, input.color.b);
    float4 blendAlbedo4 = Temp6Map.Sample(sam, input.uv);
    color = lerp(color, blendAlbedo4, input.color.a);
    color = pow(color, 2.2f);
    
    float3 viewNormal = input.normalV;
    float3 finalTangentSpaceNormal;
    float3 tangentSpaceNormal = NormalMap.Sample(sam, input.uv).xyz;
    float3 tangentSpaceNormal2 = EmissionMap.Sample(sam, input.uv).xyz;
    float3 tangentSpaceNormal3 = Temp1Map.Sample(sam, input.uv).xyz;
    float3 tangentSpaceNormal4 = Temp4Map.Sample(sam, input.uv).xyz;
    float3 tangentSpaceNormal5 = Temp7Map.Sample(sam, input.uv).xyz;
    finalTangentSpaceNormal = lerp(tangentSpaceNormal, tangentSpaceNormal2, input.color.r);
    finalTangentSpaceNormal = lerp(finalTangentSpaceNormal, tangentSpaceNormal3, input.color.g);
    finalTangentSpaceNormal = lerp(finalTangentSpaceNormal, tangentSpaceNormal4, input.color.b);
    finalTangentSpaceNormal = lerp(finalTangentSpaceNormal, tangentSpaceNormal5, input.color.a);
        
    // [0, 1] 범위에서 [-1, 1]로 변환
    finalTangentSpaceNormal = (finalTangentSpaceNormal - 0.5f) * 2.f;
    float3x3 matTBN = { input.tangentV, input.biNormalV, input.normalV };
    viewNormal = normalize(mul(finalTangentSpaceNormal, matTBN));
    
    if (UseTexture(useARM) == 1)
    {
        float3 arm = ARMMap.Sample(sam, input.uv);
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
    
    output.color = color * materialColor;
    
    float4 projPos = { 0, 0, 0, 0 };
    
    projPos = mul(input.posV, PTM);
    
    float depth = projPos.z / projPos.w;
    
    output.depth = float4(depth, depth, depth, depth);
    
    return output;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default
