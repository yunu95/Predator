#pragma once
#include "IUnknown.h"
#include "YunuGIMatrix4x4.h"

namespace yunuGI
{
    class ISpotLight: public yunuGI::ILight
    {
    public:
        virtual void SetLightDiffuseColor(yunuGI::Color& color) override {};
        virtual void SetRange(float range) = 0;
        virtual void SetAngle(float angle) = 0;
    };
	struct ISpotLightDesc
	{
	};
}
