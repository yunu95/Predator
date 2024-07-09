#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// Temp0Map : DownSample Texture

float GetBloomCurve(float x, float threshold)
{
    float result = x;
    x *= 2.0f;
    
    result = x * 0.05 + max(0, x - threshold) * 0.5; // default threshold = 1.26
    
    return result * 0.5f;
}



float4 main(PixelIn input) : SV_TARGET
{
    float width = windowWidth / 8.f;
    float height = windowHeight / 8.f;
    float2 texelSize = 1.0 / float2(width, height);

    // 가중치를 적용하여 픽셀 값을 계산
    float4 color = 0.0;
    float weights[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
    float4 tempColor = Temp0Map.Sample(sam, input.uv);
    if (tempColor.x == 0.f && tempColor.y == 0.f && tempColor.z == 0.f)
    {
        clip(-1);
    }
    
    [unroll]
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float2 offset = texelSize * float2(i, j);
            color += Temp0Map.Sample(sam, input.uv + offset) * weights[(i + 1) * 3 + (j + 1)];
        }
    }

    // 가중치의 정규화
    color /= 9.0;
    
    //float x = (color.r + color.g + color.b) / 3.f;
    //x = GetBloomCurve(x, 1.26);

    //color *= x;
    
    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default