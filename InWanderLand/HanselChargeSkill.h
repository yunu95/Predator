#pragma once
#include "Skill.h"

class HanselChargeSkill : public Skill
{
public:
    Vector3d targetPos;
    HanselChargeSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::HANSEL_Q; }
    virtual coroutine::Coroutine operator()()override;
};
