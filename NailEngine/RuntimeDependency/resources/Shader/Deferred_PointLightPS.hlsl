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
    float4 ambient : SV_Target1;
    float4 lightShadow : SV_Target2;
};

// Deferred_PointLight
// Albedo : Deferred Albedo
// ARMMap : Deferred ARM
// Temp0Map : View Position
// Temp1Map : View Normal
// Temp2Map : Util
// temp_int0 : light index
// Mesh : Sphere

PS_OUT main(PixelIn input)
{
    PS_OUT output = (PS_OUT) 0;
    
     // input.pos = SV_Position = Screen 좌표
    float2 uv = float2(input.pos.x / windowWidth, input.pos.y / windowHeight);
    float3 viewPos = Temp0Map.Sample(sam, uv).xyz;
    if (viewPos.z <= 0.f)
    {
        clip(-1);
    }

    int lightIndex = temp_int0;
    float3 viewLightPos = mul(float4(lights[lightIndex].position.xyz, 1.f), VTM).xyz;
    float distance = length(viewPos - viewLightPos);
   
    if (distance > lights[lightIndex].range)
    {
        clip(-1);
    }
    
    float3 viewNormal = Temp1Map.Sample(sam, uv).xyz;
    
    LightColor color;
    
    float3 arm;
    float3 albedo;
    if (UseTexture(useARM) == 1)
    {
        arm = ARMMap.Sample(sam, uv).xyz;
        albedo = AlbedoMap.Sample(sam, uv).xyz;
    }
    
    float4 util = Temp2Map.Sample(sam, input.uv);
    
    float shadowFactor = 1.f;
    CalculatePBRLight(temp_int0, viewNormal, viewPos, color.diffuse, color.ambient, color.specular, albedo,
    arm.r, arm.b, arm.g, util.y, util.z, util.x, shadowFactor);
    
    //CalculateLight(lightIndex, viewNormal, viewPos, color.diffuse, color.ambient, color.specular);
    
    if (util.x != -1 && useLightMap)
    {
        output.ambient = color.ambient;
    }
    else
    {
        output.diffuse = color.diffuse;
        output.ambient = color.ambient;
    }
    
    output.diffuse.w = 1;
    output.ambient.w = 1;
    
    output.lightShadow = float4(1 - shadowFactor, 1 - shadowFactor, 1 - shadowFactor, 1.f);
    
    return output;
}

// ShaderInfo
// ShaderType : Light
// RasterType : Light
// CullType : CullFront
// DepthType : NoDepthTestNoWrite
// BlendType : Light
// Sampler : Shadow