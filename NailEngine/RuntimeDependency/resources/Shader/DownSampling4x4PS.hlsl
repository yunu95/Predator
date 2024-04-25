#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// Temp0Map : Emissive Texture

float4 main(PixelIn input) : SV_Target
{
    //float4 color = 0.0;

    //// 4x4 다운샘플링
    //for (int i = 0; i < 4; ++i)
    //{
    //    for (int j = 0; j < 4; ++j)
    //    {
    //        float2 offset = float2(i, j) * 0.25; // 샘플링 오프셋 계산
    //        color += Temp0Map.Sample(sam, input.uv + offset);
    //    }
    //}

    //return color / 16.0; // 샘플의 평균값을 반환
    
    
    
    float2 texelSize = 1.0 / float2(64, 64); // 다운샘플링에 사용할 텍셀 크기 계산

    float4 color = 0.0;

    // 64x64 다운샘플링
    for (int i = 0; i < 64; ++i)
    {
        for (int j = 0; j < 64; ++j)
        {
            float2 offset = float2(i, j) * texelSize; // 샘플링 오프셋 계산
            color += Temp0Map.Sample(sam, input.uv + offset);
        }
    }

    return color / 4096.0; // 샘플의 평균값을 반환
    
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default