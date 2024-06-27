#include "Buffers.hlsli"
#include "Utils.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float2 lightUV : TEXCOORD1;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    uint4 indices : BLENDINDICES;
    float4 weight : BLENDWEIGHT;
    
    uint instanceID : SV_INSTANCEID;
    row_major matrix world : INST;
};

struct VertexOut
{
    float4 pos : POSITION;
};

row_major matrix GetAnimationMatrix(VertexIn input)
{

    
    float indices[4] = { input.indices.x, input.indices.y, input.indices.z, input.indices.w };
    float weights[4] = { input.weight.x, input.weight.y, input.weight.z, input.weight.w };

    int animIndex[2];
    int currFrame[2];
    int nextFrame[2];
    float ratio[2];

    animIndex[0] = transitionDesc[input.instanceID].curr.animIndex;
    currFrame[0] = transitionDesc[input.instanceID].curr.currFrame;
    nextFrame[0] = transitionDesc[input.instanceID].curr.nextFrame;
    ratio[0] = transitionDesc[input.instanceID].curr.ratio;

    animIndex[1] = transitionDesc[input.instanceID].next.animIndex;
    currFrame[1] = transitionDesc[input.instanceID].next.currFrame;
    nextFrame[1] = transitionDesc[input.instanceID].next.nextFrame;
    ratio[1] = transitionDesc[input.instanceID].next.ratio;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    matrix curr = 0;
    matrix next = 0;
    matrix transform = 0;

    for (int i = 0; i < 4; i++)
    {
        c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], animIndex[0], 0));
        c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], animIndex[0], 0));
        c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], animIndex[0], 0));
        c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], animIndex[0], 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], animIndex[0], 0));
        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], animIndex[0], 0));
        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], animIndex[0], 0));
        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], animIndex[0], 0));
        next = matrix(n0, n1, n2, n3);

        matrix result = lerp(curr, next, ratio[0]);

		// 다음 애니메이션
        if (animIndex[1] >= 0)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], animIndex[1], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], animIndex[1], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], animIndex[1], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], animIndex[1], 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], animIndex[1], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], animIndex[1], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], animIndex[1], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], animIndex[1], 0));
            next = matrix(n0, n1, n2, n3);

            matrix nextResult = lerp(curr, next, ratio[1]);
            result = lerp(nextResult, result, transitionDesc[input.instanceID].transitionRatio);
        }

        transform += mul(weights[i], result);
    }

    return transform;
}

VertexOut main(VertexIn input)
{
    VertexOut output = (VertexOut) 0;
    
    if (transitionDesc[input.instanceID].curr.animIndex == -1)
    {
        output.pos = mul(float4(input.pos, 1.f), input.world);
    }
    else
    {
        row_major matrix boneMat = GetAnimationMatrix(input);
        
        output.pos = mul(float4(input.pos, 1.f), boneMat);
        output.pos = mul(output.pos, input.world);
    }
    
    return output;
}