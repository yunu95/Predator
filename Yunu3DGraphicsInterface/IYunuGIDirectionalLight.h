#pragma once
#include "ILight.h"
#include "YunuGIMatrix4x4.h"

namespace yunuGI
{
    struct Matrix4x4;
    class IDirectionalLight : public yunuGI::ILight
    {
    public:
        virtual void SetLightDiffuseColor(yunuGI::Color& color) override {};
        virtual ~IDirectionalLight() = default;

    };
    struct IDirectionalLightDesc
    {
    };
}
