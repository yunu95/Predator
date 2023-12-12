#pragma once

#include "YunuGIMatrix4x4.h"
#include "YunuGIColor.h"
#include "IUnknown.h"
#include <string>

namespace yunuGI
{
    class IMaterial;

    class IRenderable : virtual public yunuGI::IUnknown
    {
    public:
        virtual void SetWorldTM(const Matrix4x4& tm) =0;
        virtual void SetActive(bool isActive) =0;
        //virtual void SetMaterial(IMaterial* material) = 0;
        //virtual IMaterial* GetMaterial() = 0;

    };
}
