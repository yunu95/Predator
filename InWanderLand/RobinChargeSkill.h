#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class RobinChargeSkill : public Skill
{
public:
    Vector3d targetPos;
    RobinChargeSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;
private:
    std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
};

