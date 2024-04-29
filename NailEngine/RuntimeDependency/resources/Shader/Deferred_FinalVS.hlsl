struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 lightUV : TEXCOORD1;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut) 0;
    
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;
    
    return output;
}