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
    uint useTemp2;
    
    int temp_int0;
    int temp_int1;
    int temp_int2;
    int temp_int3;
    int temp_int4;
    int temp_int5;
    int temp_int6;
    int temp_int7;
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
    LightColor color;
    float4 position;
    float4 direction;
    int lightType;
    float range;
    float angle;
    int padding;
};

cbuffer LightParams : register(b2)
{
    unsigned int lightCount;
    float3 padding;
    LightInfo lights[50];
};

//

cbuffer CameraBuffer : register(b3)
{
    float3 cameraPos;
    float pad;
};

cbuffer BoneMatrixBuffer : register(b4)
{
    row_major matrix BoneTransform[96];
}

Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D ARMMap : register(t2);
Texture2D HeightMap : register(t3);
Texture2D EmissionMap : register(t4);
Texture2D Temp0Map : register(t5);
Texture2D Temp1Map : register(t6);
Texture2D Temp2Map : register(t7);



SamplerState sam : register(s0);

#endif