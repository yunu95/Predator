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
    
    //for (int i = 0; i < lightCount; ++i)
    //{
    //    CalculateLight(i, viewNormal, float4(viewPos, 0.f), color.diffuse, color.ambient, color.specular);
    //}
    
    CalculateLight(temp_int0, viewNormal, viewPos, color.diffuse, color.ambient, color.specular);
    
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
// Sampler : Default