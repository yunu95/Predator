#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class EnemyImpaleSkill : public Skill
{
public:
    Vector3d targetPos;
    EnemyImpaleSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual coroutine::Coroutine operator()() override;
    virtual void OnInterruption() override;
private:
    struct Spear
    {
        Vector2d position;
        float timeOffset;
    };
    coroutine::Coroutine SpearArise(std::weak_ptr<EnemyImpaleSkill> skill, Vector2d pos, float timeOffset);
    std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
    std::unordered_set<Unit*> damagedUnits;
    std::vector<Spear> spears;
};
