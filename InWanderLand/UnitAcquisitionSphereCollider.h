#pragma once
#include "YunutyEngine.h"

class Unit;
class UnitAcquisitionSphereCollider : public physics::SphereCollider
{
public:
    // nullable
    std::weak_ptr<Unit> owner;
    int teamIndex{ 0 };
    // 콜라이더에 닿은 모든 유닛과 owner 기준 적, 아군 유닛들의 목록
    const std::unordered_set<Unit*>& GetUnits() { return units; }
    const std::unordered_set<Unit*>& GetFoes() { return foes; }
    const std::unordered_set<Unit*>& GetFriends() { return allies; }
    bool includeDeadUnits = false;
    bool includeInvulnerableUnits = false;
protected:
    virtual void OnUnitEnter(Unit* unit) {};
    virtual void OnUnitExit(Unit* unit) {};
    virtual void OnHostileEnter(Unit* hostile) {};
    virtual void OnHostileExit(Unit* hostile) {};
    virtual void OnAllyEnter(Unit* ally) {};
    virtual void OnAllyExit(Unit* ally) {};
    virtual void Update() override;
    int GetTeamIndex();

    std::unordered_set<Unit*> units;
    std::unordered_set<Unit*> foes;
    std::unordered_set<Unit*> allies;
private:
    virtual void OnTriggerEnter(physics::Collider* other) override;
    virtual void OnTriggerExit(physics::Collider* other) override;
};
