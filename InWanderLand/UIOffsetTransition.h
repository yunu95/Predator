#pragma once
#include "YunutyEngine.h"
#include "UICurveType.h"
#include "Timer.h"

class UIElement;
// 게임오브젝트가 활성화될때 스케일을 0에서 확 늘리는 컴포넌트
class UIOffsetTransition : public Timer
{
public:
    void Init(const JsonUIData& jsonData, bool isEnableEffect)
    {
        Vector3d startPosition = GetTransform()->GetLocalPosition();
        Vector3d endPosition = GetTransform()->GetLocalPosition();
        float delay = 0;

        if (isEnableEffect)
        {
            startPosition += Vector3d{ jsonData.enableOffset[0],-jsonData.enableOffset[1], 0 };
            delay = jsonData.enableDelay;
            m_duration = delay + jsonData.enableDuration;
        }
        else
        {
            endPosition += Vector3d{ jsonData.disableOffset[0],-jsonData.disableOffset[1], 0 };
            delay = jsonData.disableDelay;
            m_duration = delay + jsonData.disableDuration;
        }
        isRealtime = true;
        onCompleteFunction = [=]()
            {
                GetTransform()->SetLocalPosition(endPosition);
                GetGameObject()->SetSelfActive(isEnableEffect);
            };
        onUpdate = [=](float t)
            {
                t = max(0.0f, (t * m_duration - delay) / (m_duration - delay));
                GetTransform()->SetLocalPosition(Vector3d::Lerp(startPosition, endPosition, t));
            };
        onActivation = [=]()
            {
                GetTransform()->SetLocalPosition(startPosition);
                GetGameObject()->SetSelfActive(true);
            };
    }
};