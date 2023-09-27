#ifndef _GLOBAL_HLSLI_
#define _GLOBAL_HLSLI_

cbuffer MatrixBuffer : register(b0)
{
    matrix WTM;
    matrix VTM;
    matrix PTM;
}

cbuffer MaterialBuffer : register(b1)
{
    float4 materialColor;
};

#endif