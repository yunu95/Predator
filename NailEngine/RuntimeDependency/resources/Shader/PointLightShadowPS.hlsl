struct PixelIn
{
    float4 Pos : SV_POSITION; // 이 부분을 GS에서 출력한 Pos로 변경
    uint RTIndex : SV_RenderTargetArrayIndex; // GS에서 출력한 RTIndex 추가
};

void main(PixelIn input)
{
    return;
}

// ShaderInfo
// ShaderType : Deferred
// RasterType : Shadow
// CullType : CullBack
// DepthType : LessEqual
// BlendType : Default
// Sampler : Default