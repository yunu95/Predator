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
    ////////////if (lightPower.x == 0.f && lightPower.y == 0.f && lightPower.z == 0.f)
    ////////////{
    ////////////    clip(-1);
    ////////////}
    
    //float4 color = Temp0Map.Sample(sam, input.uv);
    //color = saturate((color * (a * color + b)) / (color * (c * color + d) + e));
    //output = pow(output, 2.2f);
    //lightPower = pow(float4(lightPower.xyz, 1.f), 1 / 2.2f);
    //lightPower = pow(lightPower, 1/2.2f);
    
    //float3 x = max(0, lightPower.xyz - 0.004);
    //lightPower.xyz = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    //lightPower.w = 1;
    //x = pow(x, 1 / 2.2);
    
    float4 specular = Temp2Map.Sample(sam, input.uv);
    float4 emissive = Temp3Map.Sample(sam, input.uv);
    //output = lightPower + emissive + specular;
    //output = float4(x.xyz,1.f) + emissive + specular;
    
    output = Temp0Map.Sample(sam, input.uv);
    
    return output;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default