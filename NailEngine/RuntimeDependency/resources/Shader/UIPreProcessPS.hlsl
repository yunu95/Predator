Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

struct PixelIn
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float2 clippingDirection : TEXCOORD1;
    float clippingThreshold : TEXCOORD2;
    float2 linearClippingDirection : TEXCOORD3;
    float2 linearClippingStart : TEXCOORD4;
    float2 capsulePoints : TEXCOORD5;
};

float4 main(PixelIn input) : SV_TARGET
{
    clip(dot(normalize(float2(input.uv.x - 0.5, (1 - input.uv.y) - 0.5)), normalize(input.clippingDirection)) - input.clippingThreshold - 0.000001);
    //clip(-dot(normalize(float2(input.uv.x - 0.5, (1 - input.uv.y) - 0.5) - float2(input.linearClippingStart.x - 0.5, (1 - input.linearClippingStart.y))), normalize(input.linearClippingDirection)) - 0.000001);
    input.linearClippingDirection.y = -input.linearClippingDirection.y;
    input.linearClippingStart.y = 1 - input.linearClippingStart.y;
    clip(-dot(input.uv - input.linearClippingStart, normalize(input.linearClippingDirection)));
    if (input.capsulePoints.x > -0.5f && input.capsulePoints.y > -0.5f)
    {
        bool insideRectangle = false;
        insideRectangle = (input.uv.x < 0.5f + input.capsulePoints.x && input.uv.x > 0.5f - input.capsulePoints.x) ||
                          (input.uv.y < 0.5f + input.capsulePoints.y && input.uv.y > 0.5f - input.capsulePoints.y);
        if (!insideRectangle)
        {
            float2 center1 = 0.5f + input.capsulePoints;
            float2 center2 = 0.5f - input.capsulePoints;
            float radius = min(0.5f - input.capsulePoints.x, 0.5f - input.capsulePoints.y);
            float2 normalizer = 1 / (1 - 2 * input.capsulePoints);
            clip(0.5f - min(length((input.uv - center1) * normalizer), length((input.uv - center2) * normalizer)));
        }
    }
    // TODO
    return Texture.Sample(TextureSampler, input.uv);
}

// ShaderInfo
// ShaderType : Forward
// RasterType : Solid
// CullType : CullBack
// DepthType : Less
// BlendType : Default
// Sampler : Default