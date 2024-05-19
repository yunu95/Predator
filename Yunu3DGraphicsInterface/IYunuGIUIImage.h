#pragma once
#include <windows.h>
#include "IYunuGIUIObject.h"

namespace yunuGI
{
    class ITexture;
    class IUIImage : public yunuGI::IUIObject
    {
    public:
        virtual yunuGI::ITexture* GetImage() = 0;
        virtual void SetImage(ITexture* texture) = 0;
        virtual float GetWidth() = 0;
        virtual float GetHeight() = 0;
        virtual void SetWidth(float width) = 0;
        virtual void SetHeight(float height) = 0;
        virtual float GetXPivot() = 0;
        virtual float GetYPivot() = 0;
        virtual void SetXPivot(float xPivot) = 0;
        virtual void SetYPivot(float yPivot) = 0;
        virtual int GetLayer() = 0;
        virtual yunuGI::Color GetColor() = 0;
        virtual void SetColor(const yunuGI::Color& color) = 0;
        virtual bool IsRadialFillMode() = 0;
        virtual void SetVideo(const std::wstring& resourcePath) = 0;
        // 텍스처가 중심에서부터 갉아먹는 형태로 채워지는지 여부를 설정
        virtual void SetRadialFillMode(bool fill) = 0;
        virtual void SetRadialFillDegree(float radian) = 0;
        virtual void SetRadialFillStartPoint(float x, float y) = 0;
        // 시계방향으로 채우는지 반시계 방향으로 채우는지 설정
        virtual void SetRadialFillDirection(bool isClockwise) = 0;
        virtual bool IsLinearClippingMode() = 0;
        virtual void SetLinearClipping(bool clip) = 0;
        virtual void SetLinearClippingStartPoint(float x, float y) = 0;
        virtual void SetLinearClippingDirection(float x, float y) = 0;
    };
    struct IUIImageDesc
    {

    };
}
