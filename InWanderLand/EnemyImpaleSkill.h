#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class EnemyImpaleSkill : public Skill
{
public:
    EnemyImpaleSkill() {}
    virtual coroutine::Coroutine operator()() override;
    virtual void OnInterruption() override;
private:
    coroutine::Coroutine SpearArise(std::weak_ptr<EnemyImpaleSkill> skill, std::weak_ptr<ManagedFBX> fbx, Vector2d pos);
    std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
    std::unordered_set<Unit*> damagedUnits;
    //static const std::vector<Spear> SpearsInfo();
};
