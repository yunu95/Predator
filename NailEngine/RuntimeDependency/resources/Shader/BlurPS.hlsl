#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// Temp0Map : DownSample Tecture

float4 main(PixelIn input) : SV_TARGET
{
    float2 texelSize = 1.0 / float2(80.f, 45.f);

    // 가중치를 적용하여 픽셀 값을 계산
    float4 color = 0.0;
    float weights[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };

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

    return color;
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default