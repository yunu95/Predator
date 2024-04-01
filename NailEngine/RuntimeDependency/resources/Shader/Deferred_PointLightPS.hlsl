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

// Deferred_PointLight
// Albedo : Deferred Albedo
// ARMMap : Deferred ARM
// Temp0Map : View Position
// Temp1Map : View Normal
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
    
    CalculatePBRLight(temp_int0, viewNormal, viewPos, color.diffuse, color.ambient, color.specular, albedo, arm.r, arm.b, arm.g);
    //CalculateLight(lightIndex, viewNormal, viewPos, color.diffuse, color.ambient, color.specular);
    
    //float3 x = max(0, color.diffuse.xyz - 0.004);
    //color.diffuse.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    //color.diffuse.w = 1;
    
    output.diffuse = (color.diffuse + color.ambient);
    output.diffuse.w = 1;
    //output.specular = color.specular;
    
    return output;
}

// ShaderInfo
// ShaderType : Light
// RasterType : Light
// CullType : CullFront
// DepthType : NoDepthTestNoWrite
// BlendType : Light
// Sampler : Shadow