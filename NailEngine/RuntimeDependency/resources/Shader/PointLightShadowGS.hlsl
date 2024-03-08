#include "Buffers.hlsli"

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
 void main(triangle float4 InPos[3] : SV_Position, inout TriangleStream<GS_OUTPUT> OutStream)
{
    for (int iFace = 0; iFace < 6; iFace++)
    {
        GS_OUTPUT output;
        output.RTIndex = iFace;
        for (int v = 0; v < 3; v++)
        {
            output.Pos = mul(InPos[v], viewProj[iFace]);
            OutStream.Append(output);
        }
        OutStream.RestartStrip();
    }
}