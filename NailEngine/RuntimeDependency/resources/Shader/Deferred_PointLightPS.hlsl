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
    
     // input.pos = SV_Position = Screen ÁÂÇ¥
    float2 uv = float2(input.pos.x / 1200, input.pos.y / 800);
    float3 viewPos = Temp0Map.Sample(sam, uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);

    int lightIndex = temp_int0;
    float3 viewLightPos = mul(float4(lights[lightIndex].position.xyz, 1.f), VTM).xyz;
    float distance = length(viewPos - viewLightPos);
    if (distance > lights[lightIndex].range)
        clip(-1);
    
    float3 viewNormal = Temp1Map.Sample(sam, uv).xyz;
    
    LightColor color;
    
    for (int i = 0; i < lightCount; ++i)
    {
        CalculateLight(i, viewNormal, float4(viewPos, 0.f), color.diffuse, color.ambient, color.specular);
    }
    
    output.diffuse = color.diffuse + color.ambient;
    //output.specular = color.specular;
    
    return output;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Solid
// CullType : CullBack
// DepthType : Less