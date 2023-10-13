#pragma once
#include "IUnknown.h"
#include "YunuGIMatrix4x4.h"

namespace yunuGI
{
    class ISpotLight: public yunuGI::ILight
    {
    public:
        virtual void SetLightDiffuseColor(yunuGI::Color& color) override {};
    };
}
