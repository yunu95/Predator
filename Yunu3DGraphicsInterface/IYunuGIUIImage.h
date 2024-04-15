#pragma once
#include <windows.h>
#include "IYunuGIUIObject.h"

namespace yunuGI
{
    class ITexture;
    class IUIImage : public yunuGI::IUIObject
    {
    public:
        virtual void SetImage(ITexture* texture) = 0;
        virtual float GetWidth() = 0;
        virtual float GetHeight() = 0;
        virtual void SetWidth(float width) = 0;
        virtual void SetHeight(float height) = 0;
        virtual int GetLayer() = 0;
    };
    struct IUIImageDesc
    {

    };
}
