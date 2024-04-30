#pragma once
#include "FloatFollower.h"

void FloatFollower::Update()
{
    currentFloat = math::LerpF(currentFloat, targetFloat, Time::GetDeltaTimeUnscaled() * followingRate);
    if (applier)
    {
        applier(currentFloat);
    }
}
void FloatFollower::SetCurrentFloat(float value)
{
    currentFloat = value;
}
void FloatFollower::SetFollowingRate(float followingRate)
{
    this->followingRate = followingRate;
    if (followingRate == 0)
        justApplyit = true;
}
void FloatFollower::SetTargetFloat(float target)
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

void FloatFollower::PlayFunction()
{

}

void FloatFollower::StopFunction()
{
	yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
}
