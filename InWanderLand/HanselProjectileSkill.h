#pragma once
#include "Skill.h"

class HanselProjectileSkill : public Skill
{
private:
    coroutine::Coroutine ThrowingPie();
    std::weak_ptr<UnitAcquisitionSphereCollider> pieCollider;

    std::unordered_set<Unit*> onceCollidedUnits;

public:
    Vector3d targetPos;
    HanselProjectileSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::HANSEL_W; }
    virtual coroutine::Coroutine operator()() override;
    virtual void OnInterruption() override;
};
