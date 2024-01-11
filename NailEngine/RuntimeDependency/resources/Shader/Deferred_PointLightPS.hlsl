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
// Temp0Map : View Position
// Temp1Map : View Normal
// temp_int0 : light index
// Mesh : Sphere

PS_OUT main(PixelIn input)
{
    PS_OUT output = (PS_OUT) 0;
    
     // input.pos = SV_Position = Screen 좌표
    float2 uv = float2(input.pos.x / 1920, input.pos.y / 1080);
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
    
    CalculateLight(lightIndex, viewNormal, viewPos, color.diffuse, color.ambient, color.specular);
    
    output.diffuse = (color.diffuse + color.ambient);
    //output.specular = color.specular;
    
    return output;
}

// ShaderInfo
// ShaderType : Light
// RasterType : Solid
// CullType : CullFront
// DepthType : NoDepthTestNoWrite
// BlendType : AlphaBlend
// Sampler : Default