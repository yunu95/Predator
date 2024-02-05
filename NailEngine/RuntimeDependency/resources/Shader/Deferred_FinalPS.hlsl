#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// Deferred_Final
// Temp0Map : Albedo
// Temp1Map : Diffuse Light
// Temp2Map : Specular Light
// Temp2Map : Emissive Map

float4 main(PixelIn input) : SV_Target
{
    float4 output = (float4) 0;
    
    float4 lightPower = Temp1Map.Sample(sam, input.uv);
    if (lightPower.x == 0.f && lightPower.y == 0.f && lightPower.z == 0.f)
    {
        clip(-1);
    }

    float4 color = Temp0Map.Sample(sam, input.uv);
    float4 specular = Temp2Map.Sample(sam, input.uv);
    float4 emissive = Temp3Map.Sample(sam, input.uv);
    output = lightPower + emissive +specular;
    //output = (lightPower) + specular;
    
    
    //float4 albedo = Temp1Map.Sample(sam, input.uv);
    //output = (albedo * color) + specular;
    
    return output;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default