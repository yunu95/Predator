#pragma once
#include "Skill.h"

class UrsulaQSkill : public Skill
{
private:
    std::vector<UnitAcquisitionSphereCollider*> projectiles;

public:
    Vector3d targetPos;

    UrsulaQSkill();
    UrsulaQSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual coroutine::Coroutine operator()() override;
    virtual void OnFinishSuccessful() override;
    virtual void OnFinishInterrupted() override;

    coroutine::Coroutine ShootProjectile(UnitAcquisitionSphereCollider* projectileCollider);
};

