#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class RobinTauntSkill : public Skill
{
public:
    Vector3d targetPos;
    RobinTauntSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::ROBIN_W; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;
private:
    std::weak_ptr<UnitAcquisitionSphereCollider> tauntCollider;
};

