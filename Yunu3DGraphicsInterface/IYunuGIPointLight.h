#pragma once
#include "ILight.h"
#include "YunuGIMatrix4x4.h"

namespace yunuGI
{
    class IPointLight: public yunuGI::ILight
    {
    public:
        virtual void SetLightDiffuseColor(yunuGI::Color& color) override {};
        virtual void SetRange(float range) = 0;
        virtual ~IPointLight() = default;
    };

	struct IPointLightDesc
	{
	};
}
