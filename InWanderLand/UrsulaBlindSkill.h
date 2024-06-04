#pragma once
#include "Skill.h"

class UrsulaBlindSkill : public Skill
{
public:
    Vector3d targetPos;
    UrsulaBlindSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::URSULA_Q; }
    virtual coroutine::Coroutine operator()()override;
};

