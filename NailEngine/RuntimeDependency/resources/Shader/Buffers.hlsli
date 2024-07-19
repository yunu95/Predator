#ifndef _BUFFERS_HLSLI_
#define _BUFFERS_HLSLI_

#define MAX_BONE_COUNT 250
#define MAX_FRAME_COUNT 500
#define MAX_INSTANCE_MODEL 500
#define MAX_PARTICLE 500
#define MAX_STATIC_MODEL 1024

cbuffer MatrixBuffer : register(b0)
{
    row_major matrix WTM;
    row_major matrix VTM;
    row_major matrix PTM;
    row_major matrix WVP;
    row_major matrix WorldInvTrans;
    row_major matrix VTMInv;
    row_major matrix lightVP;
    float4 projInvVec;
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
    uint useTemp4;
    uint useTemp5;
    uint useTemp6;
    uint useTemp7;
    uint useTemp8;
    uint useTemp9;
    uint useTemp10;
    uint useTemp11;
    
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
    
    float temp_float0;
    float temp_float1;
    float temp_float2;
    float temp_float3;
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
    float farPlane;
    float nearPlane;
    float intensity;
    float2 padding;
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

cbuffer PointLightIndex : register(b8)
{
    int plIndex;
    int isShadowCast;
    float2 padding3;
}

cbuffer ExposureBuffer : register(b9)
{
    float DiffuseExposure;
    float AmbientExposure;
    float depthBias;
    float pad4;
}

cbuffer UtilBuffer : register(b10)
{
    float windowWidth;
    float windowHeight;
    float deltaTime;
    int useIBL;
    
    int useLightMap;
    float3 padding5;
}

struct ParticleDesc
{
    float3 pos;
    float scale;
    float particleAngle;
    float alpha;
    float2 pad9;
};

cbuffer ParticleBuffer : register(b11)
{
    ParticleDesc particleDesc[MAX_PARTICLE];
};

struct LightMapUV
{
    float lightMapIndex;
    int isOutLine;
    float2 pad;
    float2 uvOffset;
    float2 scaling;
    float4 outlineInfo;
};

cbuffer LightMapUVBuffer : register(b12)
{
    LightMapUV lightMapUV[MAX_STATIC_MODEL];
    int castDecal;
    float3 pad7;
};

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
Texture2D Temp4Map : register(t10);
Texture2D Temp5Map : register(t11);
Texture2D Temp6Map : register(t12);
Texture2D Temp7Map : register(t13);
Texture2D Temp8Map : register(t14);
Texture2D Temp9Map : register(t15);
Texture2D Temp10Map : register(t16);
Texture2D Temp11Map : register(t17);

Texture2DArray TransformMap : register(t18);
TextureCube CubeMap: register(t19);
TextureCube IrradianceMap : register(t20);
TextureCube PrefilteredMap : register(t21);
Texture2D BrdfMap : register(t22);
TextureCubeArray PointLightShadowMap : register(t23);


Texture2DArray<float4> UnityLightMap : register(t24);
Texture2D Early_Z : register(t25);

SamplerState sam : register(s0);
SamplerComparisonState shadowSam : register(s1);
SamplerState spBRDF_Sampler : register(s2);

#endif