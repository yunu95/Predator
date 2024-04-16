#pragma once
#include "YunutyEngine.h"
#include "Timer.h"
//#include "curve.h"

// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class PopupOnEnable : public Timer
{
    constexpr static float popupDuration = 1.3f;
    virtual void OnEnable() override
    {
        m_duration = popupDuration;
        onUpdate = [this](float t)
            {
                //GetTransform()->SetLocalScale(Vector3d::one * math::EaseOutQuad.Evaluate((double)normalizedT));
                GetTransform()->SetLocalScale({ (1 - (1 - t) * (1 - t)),1,1 });
            };
        ActivateTimer();
        GetTransform()->SetLocalScale(Vector3d::zero);
    }
};
