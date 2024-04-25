#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// Temp0Map : Emissive Texture

float4 main(PixelIn input) : SV_Target
{
    float4 color = 0.0;

    // 6x6 다운샘플링
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 6; ++j)
        {
            float2 offset = float2(i, j) * (1.0 / 6.0); // 샘플링 오프셋 계산
            color += Temp0Map.Sample(sam, input.uv + offset);
        }
    }

    return color / 36.0; // 샘플의 평균값을 반환
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default