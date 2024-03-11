#ifndef _BUFFERS_HLSLI_
#define _BUFFERS_HLSLI_

#define MAX_BONE_COUNT 250
#define MAX_FRAME_COUNT 500
#define MAX_INSTANCE_MODEL 500

cbuffer MatrixBuffer : register(b0)
{
    row_major matrix WTM;
    row_major matrix VTM;
    row_major matrix PTM;
    row_major matrix WVP;
    row_major matrix WorldInvTrans;
    row_major matrix VTMInv;
    row_major matrix lightVP;
    //float4 objectID;
}

cbuffer MaterialBuffer : register(b1)
{
    float4 materialColor;
    
    uint useAlbedo;
    uint useNormal;
    uint useARM;
    uint useHeight;
    uint useEmission;
    uint useOpacity;
    uint useTemp0;
    uint useTemp1;
    uint useTemp2;
    uint useTemp3;
    
    int temp_int0;
    int temp_int1;
    int temp_int2;
    int temp_int3;
    int temp_int4;
    int temp_int5;
    int temp_int6;
    int temp_int7;
    int temp_int8;
    int temp_int9;
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
    row_major matrix BoneTransform[MAX_BONE_COUNT];
}

struct KeyframeDesc
{
    int animIndex;
    uint currFrame;
    uint nextFrame;
    float ratio;
    float sumTime;
    float speed;
    float2 padding;
};

struct TransitionDesc
{
    float transitionDuration;
    float transitionSpeed;
    float transitionTotalTime;
    float transitionRatio;
    KeyframeDesc curr;
    KeyframeDesc next;
};

cbuffer InstanceTransitionDesc : register(b5)
{
    TransitionDesc transitionDesc[MAX_INSTANCE_MODEL];
}

cbuffer FogBuffer : register(b6)
{
    float fogStart;
    float fogEnd;
    float2 padding2;
}

cbuffer PointLightVPMatrix : register(b7)
{
    row_major matrix viewProj[6];
}

Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D ARMMap : register(t2);
Texture2D HeightMap : register(t3);
Texture2D EmissionMap : register(t4);
Texture2D OpacityMap : register(t5);
Texture2D Temp0Map : register(t6);
Texture2D Temp1Map : register(t7);
Texture2D Temp2Map : register(t8);
Texture2D Temp3Map : register(t9);

Texture2DArray TransformMap : register(t10);
TextureCube CubeMap: register(t11);
TextureCube IrradianceMap : register(t12);
TextureCube PrefilteredMap : register(t13);
Texture2D BrdfMap : register(t14);
TextureCube PointLightShadowMap : register(t15);

SamplerState sam : register(s0);
SamplerComparisonState shadowSam : register(s1);
SamplerState spBRDF_Sampler : register(s2);

#endif