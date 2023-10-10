#ifndef _BUFFERS_HLSLI_
#define _BUFFERS_HLSLI_

cbuffer MatrixBuffer : register(b0)
{
    row_major matrix WTM;
    row_major matrix VTM;
    row_major matrix PTM;
    row_major matrix WVP;
}

cbuffer MaterialBuffer : register(b1)
{
    float4 materialColor;
};

#endif