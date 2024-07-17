#include "Buffers.hlsli"
#include "Utils.hlsli"

struct PixelIn
{
    float4 posH : SV_Position;
    float4 posV : POSITION;
    float4 posW : POSITION2;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

float4 main(PixelIn input) : SV_Target0
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    color = AlbedoMap.Sample(sam, input.uv);
    color = pow(color, 2.2f);
    float4 vertexWorldPos = input.posW;
    vertexWorldPos.y /= 2;
    vertexWorldPos.y += temp_float0;
    vertexWorldPos.y = saturate(vertexWorldPos.y);
    
    float2 tempUV = (input.uv + float2(temp_float1, temp_float1)) * 2048;
    if (tempUV.x > 2048)
        tempUV.x = tempUV.x - 2048;
    if (tempUV.y > 2048)
        tempUV.y = tempUV.y - 2048;
    float tempUV2 = (input.uv * 2) + float2(temp_float1, temp_float1);
    float noise = Temp0Map.Sample(sam, tempUV2).r;
        
    float3 result = BlendHardLight(float3(noise, noise, noise), float3(vertexWorldPos.y, vertexWorldPos.y, vertexWorldPos.y));
    
    float clipVal = 1 - result.x;
    clipVal *= color.a;
    float maxValue = max(noise, vertexWorldPos.y);
    
    clip(clipVal - 0.3333);
  
 
    return float4(0,0,0,1);
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Shadow
// CullType : CullNone
// DepthType : Less
// BlendType : Default
// Sampler : Default
