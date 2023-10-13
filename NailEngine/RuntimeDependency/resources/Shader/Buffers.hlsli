#ifndef _BUFFERS_HLSLI_
#define _BUFFERS_HLSLI_

cbuffer MatrixBuffer : register(b0)
{
    row_major matrix WTM;
    row_major matrix VTM;
    row_major matrix PTM;
    row_major matrix WVP;
    row_major matrix WorldInvTrans;
}

cbuffer MaterialBuffer : register(b1)
{
    float4 materialColor;
    
    uint useAlbedo;
    uint useNormal;
    uint useARM;
    uint useHeight;
    uint useEmission;
    uint useTemp0;
    uint useTemp1;
};

// 라이트 관련
struct LightColor
{
    float4 diffuse;
    float4 ambient;
    float4 specular;
};

struct LightInfo
{
    unsigned int lightType;
    LightColor color;
    float4 position;
    float4 direction;
    float range;
    float angle;
    float padding;
};

cbuffer LightParams : register(b2)
{
    unsigned int lightCount;
    float3 padding;
    LightInfo lights[50];
};

//
Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);

SamplerState sam : register(s0);

#endif