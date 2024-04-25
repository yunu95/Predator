#pragma once
#include "YunutyEngine.h"
#include "Timer.h"
//#include "curve.h"

// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class PopupOnEnable : public Timer
{
public:
    constexpr static float popupDuration = 0.2f;
    void Init()
    {
        m_duration = popupDuration;
        isRealtime = true;
        onUpdate = [this](float t)
            {
                GetTransform()->SetLocalScale({ (1 - (1 - t) * (1 - t)),1,1 });
            };
        onActivation = [this]()
            {
                GetTransform()->SetLocalScale(Vector3d::zero);
            };
    }
};
