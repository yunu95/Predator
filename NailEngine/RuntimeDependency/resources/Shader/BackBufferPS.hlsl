#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// BackBuffer
// Temp0Map : Final Target
// Temp1Map : View Position
// Temp2Map : OutLine Info (r,g,b,a(objectID))
// Temp3Map : Utils (g(isOutLine))

float4 FixColor(int2 uv)
{
    float4 color = float4(-1, -1, -1, 1);

    float isOutLine = Temp3Map.Load(float3(uv.x,uv.y,0)).g;
    int outLineRange = 1;
    int filterSize = 2 * outLineRange + 1;
    float4 outLineInfo = Temp2Map.Load(float3(uv.x, uv.y, 0));
    int id = outLineInfo.a;
    if(isOutLine == 0)
    {
        return color;
    }
    else
    {
        [unroll]
        for (int i = 0; i < filterSize; ++i)
        {
            for (int j = 0; j < filterSize; ++j)
            {
                int x = i - outLineRange;
                int y = j - outLineRange;
                int arroundID = Temp2Map.Load(float3(uv.x + x, uv.y + y, 0)).a;
                if(id != arroundID)
                {
                    color.rgb = outLineInfo.xyz;
                    return color;
                }
            }
        }
    }
    return color;
}

float4 main(PixelIn input) : SV_Target
{
    float4 color = Temp0Map.Sample(sam, input.uv);
    
    
    float4 tempColor = FixColor(float2(input.uv.x * windowWidth, input.uv.y * windowHeight));
    
    if(tempColor.r != -1)
    {
        color = tempColor;
    }
    color.w = 1;
    
    
    
    float4 viewPos = Temp1Map.Sample(sam, input.uv);
    
    float fogFactor = saturate((fogEnd - (viewPos.z/ viewPos.w)) / (fogEnd - fogStart));
    float4 fogColor = fogFactor * color + (1 - fogFactor) * float4(0.7686, 0.8784, 0.9451,1.f);
    return fogColor;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default