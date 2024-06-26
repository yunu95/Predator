#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 clippingDirection : TEXCOORD1;
    float clippingThreshold : TEXCOORD2;
    float2 linearClippingDirection : TEXCOORD3;
    float2 linearClippingStart : TEXCOORD4;
    // 이미지의 가로가 4, 세로가 1이라면, 캡슐은 가로로 형성되며, 캡슐의 양쪽에 둥근 반원의 중점은 0.5, 3.5에 위치한다. 이는 중앙으로부터 1.5만큼 떨어진 위치다.
    // 중앙으로부터 떨어진 거리를 가로길이로 나누어 정규화하면 0.375가 된다. 그 경우 capusePoints는 {0.375, 0}이다.
    // capsulePoints가 -1,-1이면 캡슐 클리핑을 하지 않는다.
    float2 capsulePoints : TEXCOORD5;
};

struct VertexOut
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float2 clippingDirection : TEXCOORD1;
    float clippingThreshold : TEXCOORD2;
    float2 linearClippingDirection : TEXCOORD3;
    float2 linearClippingStart : TEXCOORD4;
    float2 capsulePoints : TEXCOORD5;
};

VertexOut main(VertexIn input)
{
    VertexOut output;
    output.uv = input.uv;
    output.pos = float4(input.pos, 1);
    output.clippingDirection = input.clippingDirection;
    output.clippingThreshold = input.clippingThreshold;
    output.linearClippingDirection = input.linearClippingDirection;
    output.linearClippingStart = input.linearClippingStart;
    output.capsulePoints = input.capsulePoints;
    return output;
}
