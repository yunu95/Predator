#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

struct PS_OUT
{
    float4 diffuse : SV_Target0;
    float4 specular : SV_Target1;
};

// Deferred_DirectionalLight
// Albedo : Deferred Albedo
// ARMMap : Deferred ARM
// Temp0Map : View Position
// Temp1Map : View Normal
// Temp2Map : Shadow Map
// temp_int0 : light index

PS_OUT main(PixelIn input)
{
    PS_OUT output = (PS_OUT) 0;
    
    float3 viewPos = Temp0Map.Sample(sam, input.uv).xyz;
    if (viewPos.z <= 0.f)
    {
        clip(-1);
    }
    
    float3 viewNormal = Temp1Map.Sample(sam, input.uv).xyz;
    //float3 viewNormal = pow(Temp1Map.Sample(sam, input.uv).xyz, 1 / 2.2f);
    
    LightColor color;
    
    float3 arm;
    float3 albedo;
    if (UseTexture(useARM) == 1)
    {
        arm = ARMMap.Sample(sam, input.uv).xyz;
        albedo = AlbedoMap.Sample(sam, input.uv).xyz;
    }
    CalculatePBRLight(temp_int0, viewNormal, viewPos, color.diffuse, color.ambient, color.specular, albedo,arm.r, arm.b, arm.g);
    //CalculateLight(temp_int0, viewNormal, viewPos, color.diffuse, color.ambient, color.specular);
    
    //float3 x = max(0, color.diffuse.xyz - 0.004);
    //color.diffuse.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    //color.diffuse.w = 1;
    
    //color.diffuse = float4(pow(float3(color.diffuse.xyz), 1.0 / 2.2), 1.0);
    output.diffuse = color.diffuse + color.ambient;
    //output.specular = color.specular;
    

    
    return output;
}

// ShaderInfo
// ShaderType : Light
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : AlphaBlend
// Sampler : Shadow