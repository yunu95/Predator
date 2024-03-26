#pragma once
#include "YunutyEngine.h"

namespace yunutyEngine
{
    // 카메라와 계속해서 각도를 맞춰주는 클래스입니다.
    class Billboard : public Component
    {
        virtual void Update() override
        {
            if (auto cam = graphics::Camera::GetMainCamera())
            {
                //static Quaternion newQuat{};
                //static Vector3d newEuler{};
                //newEuler.y += 10 * Time::GetDeltaTime();
                //auto currRot = GetTransform()->GetWorldRotation();
                //newQuat = Quaternion( newEuler );
                ////newQuat = cam->GetTransform()->GetWorldRotation();
                //GetTransform()->SetWorldRotation(newQuat);
                GetTransform()->SetWorldRotation(cam->GetTransform()->GetWorldRotation());
            }
        }
    };
}
