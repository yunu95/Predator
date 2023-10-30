#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 color : Color;
    float2 uv : TEXCOORD0;
};

float4 main(PixelIn input) : SV_TARGET
{
    return input.color * materialColor;
}

// ShaderInfo
// ShaderType : Solid
