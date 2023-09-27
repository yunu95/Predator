#include "../Global.hlsli"

struct VertexIn
{
    float4 pos : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    
    output.pos = input.pos;
    output.color = input.color;
    
    return output;
}