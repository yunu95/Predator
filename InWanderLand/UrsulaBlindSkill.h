#pragma once
#include "Skill.h"

class UrsulaBlindSkill : public Skill
{
public:
    Vector3d targetPos;
    UrsulaBlindSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::URSULA_Q; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;
private:
    std::weak_ptr<UnitAcquisitionSphereCollider> projectile;
    std::weak_ptr<UnitAcquisitionSphereCollider> circle_1;
    std::weak_ptr<UnitAcquisitionSphereCollider> circle_2;
    std::weak_ptr<UnitAcquisitionSphereCollider> circle_3;
};

