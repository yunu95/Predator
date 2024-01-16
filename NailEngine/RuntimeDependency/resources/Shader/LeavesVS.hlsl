#include "Buffers.hlsli"

struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    
    // INSTANCING
    uint instanceID : SV_INSTANCEID;
    row_major matrix world : INST;
};

struct VertexOut
{
    float4 posH : SV_Position;
    float4 posV : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normalV : NORMAL;
    float3 tangentV : TANGENT;
    float3 biNormalV : BINORMAL;
};

VertexOut main(VertexIn input)
{
    VertexOut output;

    // Calculate the direction from the vertex to the camera
    float3 toCamera = normalize(cameraPos - input.pos);

    // Calculate the dot product between the face normal and the direction to the camera
    float dotProduct = dot(input.normal, toCamera);

    // Move the vertex along the normal direction to make the face face the camera
    float3 temp = input.normal * dotProduct;
    input.pos += temp;

    
    // Calculate the transformation matrices
    row_major matrix WV = mul(input.world, VTM);
    row_major matrix VP = mul(VTM, PTM);
   
    // Transform the vertex position to homogeneous clip space
    output.posH = mul(float4(input.pos, 1.f), input.world);
    output.posH = mul(output.posH, VP);

    // Transform the vertex position to view space
    output.posV = mul(float4(input.pos, 1.f), WV);

    // Copy other attributes
    output.color = input.color;
    output.uv = input.uv;

    // Transform normals and tangents to view space
    output.normalV = normalize(mul(float4(input.normal, 0.f), WV));
    output.tangentV = normalize(mul(float4(input.tangent, 0.f), WV));
    output.biNormalV = normalize(cross(output.tangentV, output.normalV));

    return output;
}