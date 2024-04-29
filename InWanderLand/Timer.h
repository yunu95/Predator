#pragma once
#include "YunutyEngine.h"
#include "Easing.h"


class TimerComponent : public Component
{
private:
    bool m_isActivated{ false };

public:
    bool m_isRepeated{ false };

    float m_elapsed{ 0 };
    float m_duration{ 0 };
    bool isRealtime{ false };

public:

    virtual void Update() override;

public:
    std::function<void(float normalizedT)> onUpdate = [](float) {};
    std::function<void()> onActivation = []() {};
    std::function<void()> onCompleteFunction = []() {};
    std::function<void()> onExpiration = []() {};

    void ActivateTimer();
    void StopTimer();
    void SetUsingRealtime(bool);
};