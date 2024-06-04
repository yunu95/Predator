#pragma once
#include "Skill.h"

class HanselProjectileSkill : public Skill
{
public:
    Vector3d targetPos;
    HanselProjectileSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::HANSEL_W; }
    virtual coroutine::Coroutine operator()()override;
};
