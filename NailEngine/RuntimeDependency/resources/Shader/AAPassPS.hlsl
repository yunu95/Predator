#include "Buffers.hlsli"

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

// BackBuffer
// Temp0Map : BackBuffer Target -> 이 Target에 AA를 적용할 것임.

float rgb2luma(float3 rgb)
{
    return sqrt(dot(rgb, float3(0.299, 0.587, 0.114)));
}

float4 AA(float2 uv)
{
    float2 rcpFrame = 1.0 / float2(windowWidth, windowHeight);
    float3 rgbNW = Temp0Map.Sample(sam, uv + float2(-1.0, -1.0) * rcpFrame).rgb;
    float3 rgbNE = Temp0Map.Sample(sam, uv + float2(1.0, -1.0) * rcpFrame).rgb;
    float3 rgbSW = Temp0Map.Sample(sam, uv + float2(-1.0, 1.0) * rcpFrame).rgb;
    float3 rgbSE = Temp0Map.Sample(sam, uv + float2(1.0, 1.0) * rcpFrame).rgb;
    float3 rgbM = Temp0Map.Sample(sam, uv).rgb;

    float3 luma = float3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM = dot(rgbM, luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    float range = lumaMax - lumaMin;

    if (range < 0.1)
        return float4(rgbM, 1.0);

    float lumaMinEdge = min(lumaNW, lumaNE);
    float lumaMaxEdge = max(lumaNW, lumaNE);

    float edgeVert = abs(lumaNE - lumaSW);
    float edgeHorz = abs(lumaNW - lumaSE);
    float edgeDiag = abs(lumaNW + lumaSE - (lumaSW + lumaNE));

    float2 direction;
    if (edgeVert > edgeHorz && edgeVert > edgeDiag)
    {
        direction = float2(0.0, 1.0);
    }
    else if (edgeHorz > edgeDiag)
    {
        direction = float2(1.0, 0.0);
    }
    else
    {
        direction = float2(1.0, 1.0);
    }

    float3 rgb1 = Temp0Map.Sample(sam, uv - direction * rcpFrame).rgb;
    float3 rgb2 = Temp0Map.Sample(sam, uv + direction * rcpFrame).rgb;

    return float4((rgb1 + rgb2 + rgbM) / 3.0, 1.0);
}

// 엣지 검출
float DetectEdge(Texture2D tex, SamplerState samp, float2 texCoord, float2 rcpFrame)
{
    float3 color = tex.Sample(samp, texCoord).rgb;
    
    float3 colorLeft = tex.Sample(samp, texCoord + float2(-1.0, 0.0) * rcpFrame).rgb;
    float3 colorRight = tex.Sample(samp, texCoord + float2(1.0, 0.0) * rcpFrame).rgb;
    float3 colorUp = tex.Sample(samp, texCoord + float2(0.0, -1.0) * rcpFrame).rgb;
    float3 colorDown = tex.Sample(samp, texCoord + float2(0.0, 1.0) * rcpFrame).rgb;

    float3 deltaH = abs(colorLeft - color) + abs(colorRight - color);
    float3 deltaV = abs(colorUp - color) + abs(colorDown - color);
    
    float edgeH = dot(deltaH, float3(0.299, 0.587, 0.114));
    float edgeV = dot(deltaV, float3(0.299, 0.587, 0.114));
    
    float edge = saturate(max(edgeH, edgeV) - 0.1);

    return edge;
}

// 블렌딩 가중치 계산
float4 CalculateBlendWeights(Texture2D tex, SamplerState samp, float2 texCoord, float2 rcpFrame, float edge)
{
    float2 delta = float2(rcpFrame.x, rcpFrame.y) * 2.0;
    float2 deltaHalf = delta * 0.5;

    float3 color = tex.Sample(samp, texCoord).rgb;

    float3 colorLeftHalf = tex.Sample(samp, texCoord + float2(-0.5, 0.0) * rcpFrame).rgb;
    float3 colorRightHalf = tex.Sample(samp, texCoord + float2(0.5, 0.0) * rcpFrame).rgb;
    float3 colorUpHalf = tex.Sample(samp, texCoord + float2(0.0, -0.5) * rcpFrame).rgb;
    float3 colorDownHalf = tex.Sample(samp, texCoord + float2(0.0, 0.5) * rcpFrame).rgb;

    float3 deltaHHalf = abs(colorLeftHalf - color) + abs(colorRightHalf - color);
    float3 deltaVHalf = abs(colorUpHalf - color) + abs(colorDownHalf - color);

    float edgeHHalf = dot(deltaHHalf, float3(0.299, 0.587, 0.114));
    float edgeVHalf = dot(deltaVHalf, float3(0.299, 0.587, 0.114));

    float edgeHalf = saturate(max(edgeHHalf, edgeVHalf) - 0.1);

    float weightH = edge + edgeHHalf;
    float weightV = edge + edgeVHalf;

    return float4(weightH, weightV, edge, edgeHalf);
}

float4 main(PixelIn input) : SV_Target
{
    //float4 color = Temp0Map.Sample(sam, input.uv);
    return Temp0Map.Sample(sam, input.uv);
    
    //float4 color = float4(0, 0, 0, 0);

    //// MSAA 샘플 합산
    //for (int i = 0; i < 4; i++)
    //{
    //    color += Temp0Map.Load(float3(input.uv * float2(windowWidth, windowHeight), i));
    //}

    //// 평균 샘플
    //return color /= 4;
    
    //return AA(input.uv);
    
    //float2 texCoord = input.uv;
    //float2 rcpFrame = 1.0 / float2(windowWidth, windowHeight);

    //// 엣지 검출
    //float edge = DetectEdge(Temp0Map, sam, texCoord, rcpFrame);

    //// 블렌딩 가중치 계산
    //float4 blendWeights = CalculateBlendWeights(Temp0Map, sam, texCoord, rcpFrame, edge);

    //// 최종 블렌딩
    //if (blendWeights.z < 0.1)
    //{
    //    float3 color = Temp0Map.Sample(sam, texCoord).rgb;
    //    return float4(color, 1.0); // 엣지가 없으면 원본 색상을 반환
    //}

    //float2 deltaHalf = float2(rcpFrame.x, rcpFrame.y);
    //float4 colorLeftBlend = Temp0Map.Sample(sam, texCoord + float2(-deltaHalf.x, 0.0));
    //float4 colorRightBlend = Temp0Map.Sample(sam, texCoord + float2(deltaHalf.x, 0.0));
    //float4 colorUpBlend = Temp0Map.Sample(sam, texCoord + float2(0.0, -deltaHalf.y));
    //float4 colorDownBlend = Temp0Map.Sample(sam, texCoord + float2(0.0, deltaHalf.y));

    //float4 blendedColorH = lerp(colorLeftBlend, colorRightBlend, blendWeights.x);
    //float4 blendedColorV = lerp(colorUpBlend, colorDownBlend, blendWeights.y);

    //float4 finalBlendedColor = lerp(blendedColorH, blendedColorV, 0.5);

    //return lerp(float4(Temp0Map.Sample(sam, texCoord).rgb, 1.0), finalBlendedColor, blendWeights.w);
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullNone
// DepthType : NoDepthTestNoWrite
// BlendType : Default
// Sampler : Default