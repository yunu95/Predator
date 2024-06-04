#pragma once
#include "YunutyEngine.h"

class Unit;
class UnitAcquisitionSphereCollider : public physics::SphereCollider
{
public:
    UnitAcquisitionSphereCollider();
    // nullable
    std::weak_ptr<Unit> owner;
    int teamIndex{ 0 };
    // 콜라이더에 닿은 모든 유닛과 owner 기준 적, 아군 유닛들의 목록
    const std::unordered_set<Unit*>& GetUnits() { return units; }
    const std::unordered_set<Unit*>& GetEnemies() { return enemies; }
    const std::unordered_set<Unit*>& GetFriends() { return friends; }
    void SetRadius(float radius);
    void SetColor(yunuGI::Color color);
    bool includeDeadUnits = false;
    bool includeInvulnerableUnits = false;
protected:
    virtual void OnUnitEnter(Unit* unit) {};
    virtual void OnUnitExit(Unit* unit) {};
    virtual void OnEnemyEnter(Unit* enemy) {};
    virtual void OnEnemyExit(Unit* enemy) {};
    virtual void OnFriendEnter(Unit* friendUnit) {};
    virtual void OnFriendExit(Unit* friendUnit) {};
    virtual void Update() override;
    int GetTeamIndex();

    std::unordered_set<Unit*> units;
    std::unordered_set<Unit*> enemies;
    std::unordered_set<Unit*> friends;

    GameObject* debugMesh = nullptr;
private:
    virtual void OnTriggerEnter(physics::Collider* other) override;
    virtual void OnTriggerExit(physics::Collider* other) override;
};
