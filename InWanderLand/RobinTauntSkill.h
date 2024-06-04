#pragma once
#include "Skill.h"

class RobinTauntSkill : public Skill
{
public:
    Vector3d targetPos;
    RobinTauntSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::ROBIN_W; }
    virtual coroutine::Coroutine operator()()override;
};

