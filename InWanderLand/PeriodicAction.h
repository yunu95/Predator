#pragma once
#include "YunutyEngine.h"
using namespace yunutyEngine;

class PeriodicAction : public yunutyEngine::Component
{
public:
    std::function<void()> action{ []() {} };
    float duration{ 1 };
    float elapsed{ 0 };
    virtual void Update() 
    {
        elapsed += Time::GetDeltaTime();
        if (elapsed > duration)
        {
            elapsed -= duration;
            action();
        }
    }
};
