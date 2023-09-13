#pragma once

#include "YunuGIMatrix4x4.h"
#include "YunuGIColor.h"
#include "IUnknown.h"

namespace yunuGI
{
    class IRenderable : virtual public yunuGI::IUnknown
    {
    public:
        virtual void SetWorldTM(const Matrix4x4& tm) =0;
        virtual void SetActive(bool isActive) =0;

        // 아래 함수는 추후 새로 만들어질 엔진에 맞게 수정을 해야함.
        virtual void SetColor(int materialIndex, yunuGI::Color color) =0;
        virtual void SetShader(int materialIndex, std::wstring shaderKey) =0;
        virtual void SetMaterialName(int materialIndex, std::wstring materialKey) =0;
    };
}
