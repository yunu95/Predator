#pragma once
#include "YunutyEngine.h"

namespace yunutyEngine
{
    // 카메라와 계속해서 각도를 맞춰주는 클래스입니다.
    class Billboard : virtual public Component
    {
        virtual void Update() override
        {
            if (auto cam = graphics::Camera::GetMainCamera())
            {
                GetTransform()->SetWorldRotation(cam->GetTransform()->GetWorldRotation());
            }
        }
    };
}
