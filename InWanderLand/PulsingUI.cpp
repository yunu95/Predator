#pragma once
#include "PulsingUI.h"
#include "InWanderLand.h"

void PulsingUI::Start()
{
    ActivateTimer();
}
void PulsingUI::Init()
{
    isRealtime = true;
    m_isRepeated = true;
    onUpdate = [this](float t)
        {
            TimerComponent* poppingUp = uiElement->scalePopUpTransition;
            TimerComponent* poppingDown = uiElement->scalePopDownTransition;
            if ((poppingDown && poppingDown->IsActivated()) || (poppingUp && poppingUp->IsActivated()))
            {
                m_elapsed = 0;
                return;
            }
            float delta = pulsingMax - pulsingMin;
            float scale = pulsingMin + delta * 0.5f * (1.0f - cosf(t * 2 * math::PI));
            GetTransform()->SetLocalScale(Vector3d::one * scale);
        };
    onActivation = [this]()
        {
            GetTransform()->SetLocalScale(Vector3d::one * pulsingMin);
        };
};