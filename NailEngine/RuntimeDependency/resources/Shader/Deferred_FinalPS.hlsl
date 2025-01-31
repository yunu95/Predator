#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// Deferred_Final
// Temp0Map : Albedo
// Temp1Map : Diffuse Light
// Temp2Map : LightShadow
// Temp3Map : Emissive Map
// Temp4Map : Util
// Temp5Map : Ambient Light
// Temp6Map : Specular Light
// Temp7Map : Silhouette

float4 main(PixelIn input) : SV_Target
{
    float4 output = (float4) 0;
    
    float4 diffuseLight = Temp1Map.Sample(sam, input.uv);
    float4 ambientLight = Temp5Map.Sample(sam, input.uv);
    if ((diffuseLight.x == 0.f && diffuseLight.y == 0.f && diffuseLight.z == 0.f) && (ambientLight.x == 0.f && ambientLight.y == 0.f && ambientLight.z == 0.f))
    {
        clip(-1);
    }
    
    float4 shadowFactor = Temp2Map.Sample(sam, input.uv);
    float4 emissive = Temp3Map.Sample(sam, input.uv);
    
    float4 util = Temp4Map.Sample(sam, input.uv);
    
    float4 specular = Temp6Map.Sample(sam, input.uv);
    
    if (util.x == -1)
    {
        output = diffuseLight * (1 - (shadowFactor.x)) + ambientLight + emissive;
    }
    else
    {
        float4 albedoColor = Temp0Map.Sample(sam, input.uv);
        float4 shadowColor = float4(1, 1, 1, 1);
        if (shadowFactor.x > 0)
        {
            shadowColor *= (1 - shadowFactor * 0.25);
        }

        float4 temp = (albedoColor + specular) * shadowColor;
        //float4 temp = (albedoColor) * shadowColor;

        output = temp + emissive;
    }
    output.w = 1.f;
    
    //output = Temp0Map.Sample(sam, input.uv);
    output.xyz *= Temp7Map.Sample(sam, input.uv);
    return output;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default