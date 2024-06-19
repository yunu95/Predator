#pragma once
#include "YunutyEngine.h"
#include "UnitAcquisitionCollider.h"

class Unit;
class UnitAcquisitionCollider : virtual public Component
{
public:
    // nullable
    std::weak_ptr<Unit> owner;
    int teamIndex{ 0 };
    // 콜라이더에 닿은 모든 유닛과 owner 기준 적, 아군 유닛들의 목록
    const std::unordered_set<Unit*>& GetUnits() { return units; }
    const std::unordered_set<Unit*>& GetEnemies() { return enemies; }
    const std::unordered_set<Unit*>& GetFriends() { return friends; }
    bool includeDeadUnits = false;
    bool includeInvulnerableUnits = false;
protected:
    virtual void Update() override;
    int GetTeamIndex();

    // 유닛이면 일단 집어넣는 리스트
    std::unordered_set<Unit*> unitsWhatSoEver;

    std::unordered_set<Unit*> units;
    std::unordered_set<Unit*> enemies;
    std::unordered_set<Unit*> friends;

    GameObject* debugMesh = nullptr;
private:
    bool ShouldContain(Unit* unit);
    virtual void OnTriggerEnter(physics::Collider* other) override;
    virtual void OnTriggerExit(physics::Collider* other) override;
};
