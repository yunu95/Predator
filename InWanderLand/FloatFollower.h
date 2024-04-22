#pragma once
#include "YunutyEngine.h"

class FloatFollower : public Component
{
private:
    float targetFloat = 0;
    float currentFloat = 0;
public:
    function<void(float)> applier;
    float followingRate = 1;
    bool justApplyit = false;
    virtual void Update()
    {
        currentFloat = math::LerpF(currentFloat, targetFloat, Time::GetDeltaTimeUnscaled() * followingRate);
        if (applier)
        {
            applier(currentFloat);
        }
    }
    void SetCurrentFloat(float value)
    {
        currentFloat = value;
    }
    void SetFollowingRate(float followingRate)
    {
        this->followingRate = followingRate;
        if (followingRate == 0)
            justApplyit = true;
    }
    void SetTargetFloat(float target)
    {
        targetFloat = target;
        if (justApplyit)
        {
            currentFloat = targetFloat;
            if (applier)
            {
                applier(currentFloat);
            }
        }
    }
};
