#include "../Global.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float4 color : Color;
};

float4 main(PixelIn input) : SV_TARGET
{
    return input.color;
}