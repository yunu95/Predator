#pragma once
#include "Functors.h"
#include "IRenderable.h"
#include "Texture.h"
#include "Video.h"

class UIImage : public nail::IRenderable
{
public:
    void SetTexture(yunuGI::ITexture* texture);
    yunuGI::ITexture* GetTexture();
    DirectX::XMFLOAT2 GetPrimitiveTextureSize();
    ID3D11Texture2D* GetProcessedTexture();
    ID3D11ShaderResourceView* GetSRV();
    float GetWidth();
    float GetHeight();
    float GetXScale();
    float GetYScale();
    float GetXPivot();
    float GetYPivot();
    float GetRotation();
    void SetXScale(float xScale);
    void SetYScale(float yScale);
    void SetXPivot(float xPivot);
    void SetYPivot(float yPivot);
    void SetWidth(float width);
    void SetHeight(float height);
    void SetRotation(float xPivot);
    bool operator<(const UIImage& other) const;
    bool IsUsingProcessedTexture();
    int GetLayer();
    //DirectX::SimpleMath::Vector2 pos;
    int layer = 0;
    DirectX::FXMVECTOR GetColor();
    void SetColor(const DirectX::FXMVECTOR& color);
    bool IsRadialFillMode();
    bool IsVideoPlayMode();
    void SetVideo(const std::wstring& resourcePath);
    // 텍스처가 중심에서부터 갉아먹는 형태로 채워지는지 여부를 설정
    void SetRadialFillMode(bool fill);
    void SetRadialFillDegree(float degree);
    void SetRadialFillStartPoint(float x, float y);
    // 시계방향으로 채우는가?
    void SetRadialFillDirection(bool isClockwise);
    void PreProcessTexture();
    // 텍스처를 임의의 점으로부터 특정 방향으로 선형적으로 잘라냅니다.
    bool IsLinearClippingMode();
    void SetLinearClipping(bool clip);
    void SetLinearClippingStartPoint(float x, float y);
    void SetLinearClippingDirection(float x, float y);
private:
    float rotation;
    std::weak_ptr<Video> video;
    bool isRadialFillMode{ false };
    float radialFillDegree{ 360 };
    DirectX::SimpleMath::Vector2 radialFillStartDirection{ 0,1 };
    bool radialFillIsClockwise{ true };

    bool isLinearClippingMode{ false };
    DirectX::SimpleMath::Vector2 linearClippingStart{ 0, 0.5f };
    DirectX::SimpleMath::Vector2 linearClippingDirection{ 0, 1 };

    float xScale = 1;
    float yScale = 1;
    float width{ -1 };
    float height{ -1 };
    // 0,0은 왼쪽 위
    float xPivot = 0;
    float yPivot = 0;
    DirectX::FXMVECTOR color{ 1,1,1,1 };
    yunuGI::ITexture* texture;
    // UI에 전처리가 적용된 텍스처
    Microsoft::WRL::ComPtr<ID3D11Texture2D> processedTexture{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> processedTextureRTV{ nullptr };
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> processedTextureSRV{ nullptr };
};

struct CompareSharedPtr
{
    bool operator()(const std::shared_ptr<nail::IRenderable>& lhs, const std::shared_ptr<nail::IRenderable>& rhs) const
    {
        const auto& leftImage = std::static_pointer_cast<UIImage>(lhs);
        const auto& rightImage = std::static_pointer_cast<UIImage>(rhs);

        if (leftImage->layer != rightImage->layer)
        {
            return leftImage->layer < rightImage->layer;
        }
        else
        {
            return *leftImage < *rightImage;
        }
    }
};