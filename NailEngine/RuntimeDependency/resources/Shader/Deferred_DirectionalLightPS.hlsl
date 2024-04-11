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
// Temp3Map : Util
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
    
    float4 util = Temp3Map.Sample(sam, input.uv);
    
    //albedo = float4(pow(float3(albedo.xyz), 1.0 / 2.2), 1.0);
    
    CalculatePBRLight(temp_int0, viewNormal, viewPos, color.diffuse, color.ambient, color.specular, albedo, arm.r, arm.b, arm.g, util.y, util.z);
    
    //CalculateLight(temp_int0, viewNormal, viewPos, color.diffuse, color.ambient, color.specular);
    
    if (util.x != -1 && useLightMap)
    {
        output.diffuse = color.ambient;
    }
    else
    {
        output.diffuse = color.diffuse + color.ambient;
    }
    
    output.diffuse.w = 1;
    //output.diffuse = float4(albedo, 1);
    
    //output.specular = color.specular;
    

    
    return output;
}

// ShaderInfo
// ShaderType : Light
// RasterType : Light
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Light
// Sampler : Shadow