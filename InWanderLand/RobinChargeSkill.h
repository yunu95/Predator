#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class RobinChargeSkill : public Skill
{
public:
    RobinChargeSkill() {};
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;
private:
    std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
};

