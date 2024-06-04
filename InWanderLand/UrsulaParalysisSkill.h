#pragma once
#include "Skill.h"

class UrsulaParalysisSkill : public Skill
{
public:
    Vector3d targetPos;
    UrsulaParalysisSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::URSULA_W; }
    virtual coroutine::Coroutine operator()()override;
};
