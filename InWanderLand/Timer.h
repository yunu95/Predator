#pragma once
#include "YunutyEngine.h"
#include "Easing.h"
#include "LocalTimeEntity.h"
#include "ContentsObservee.h"

class TimerComponent : public Component, public LocalTimeEntity
{
private:
    bool m_isActivated{ false };

public:
    bool m_isRepeated{ false };

    float m_elapsed{ 0 };
    float duration{ 0 };
    bool isRealtime{ false };

public:

    virtual void Update() override;
    //virtual void PlayFunction() override;
    //virtual void StopFunction() override;

public:
    std::function<void(float normalizedT)> onUpdate = [](float) {};
    std::function<void()> onActivation = []() {};
    std::function<void()> onCompleteFunction = []() {};
    std::function<void()> onExpiration = []() {};

    void ActivateTimer();
    void StopTimer();
    bool IsActivated() { return m_isActivated; };
    void SetUsingRealtime(bool);
};