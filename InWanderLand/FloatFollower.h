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
    virtual void Update();
    void SetCurrentFloat(float value);
    void SetFollowingRate(float followingRate);
    void SetTargetFloat(float target);
};
