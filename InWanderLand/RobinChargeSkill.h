#pragma once
#include "Skill.h"

class RobinChargeSkill : public Skill
{
public:
    Vector3d targetPos;
    RobinChargeSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual coroutine::Coroutine operator()()override;
};

