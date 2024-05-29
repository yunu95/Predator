#pragma once
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"
#include "YunuVector2.h"
#include "YunuVector3.h"
#include "YunuGIColor.h"

namespace yunuGI
{
    class ICamera : public IUnknown
    {
    public:
        // 카메라의 월드 위치를 전송해 view matrix를 갱신한다.
        virtual void SetWorldTM(const Matrix4x4& tm) = 0;
        // 카메라의 시야각을 갱신한다.
        virtual void SetVerticalFOV(float fov) = 0;
        virtual float GetVerticalFOV() = 0;
        // 카메라 절두체의 near 플레인까지의 거리를 정의한다.
        virtual void SetNear(float near) = 0;
        virtual float GetNear() = 0;
        // 카메라 절두체의 far 플레인까지의 거리를 정의한다.
        virtual void SetFar(float far) = 0;
        virtual float GetFar() = 0;
        // 카메라의 해상도를 조절한다.
        virtual void SetResolution(float width, float height) = 0;
        virtual void GetResolution(float* width, float* height) = 0;
        // 카메라를 메인 카메라로 만든다.
        virtual void SetAsMain() = 0;
        // 월드 위치를 받아 스크린 좌표계로 변환한다.
        // x,y는 -1~ 1 사이의 값이며, y축은 화면 위쪽, x축은 화면 오른쪽을 향한다.
        virtual yunuGI::Vector2 GetScreenPos(const yunuGI::Vector3& worldPos) = 0;
        virtual yunuGI::Vector3 GetViewPos(const yunuGI::Vector3& worldPos) = 0;

    };
    struct ICameraDesc
    {
    };
}
