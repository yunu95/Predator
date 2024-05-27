#pragma once
#include "YunutyEngine.h"

class Unit;
class UnitAcquisitionSphereCollider : public physics::SphereCollider
{
public:
    // nullable
    std::weak_ptr<Unit> owner;
    // 콜라이더에 닿은 모든 유닛과 owner 기준 적, 아군 유닛들의 목록
    const std::unordered_set<std::weak_ptr<Unit>>& GetUnits() { return units; }
    const std::unordered_set<std::weak_ptr<Unit>>& GetFoes() { return foes; }
    const std::unordered_set<std::weak_ptr<Unit>>& GetFriends() { return allies; }
    bool includeDeadUnits = false;
protected:
    virtual void OnUnitEnter(std::weak_ptr<Unit> unit) {};
    virtual void OnUnitExit(std::weak_ptr<Unit> unit) {};
    virtual void OnHostileEnter(std::weak_ptr<Unit> hostile) {};
    virtual void OnHostileExit(std::weak_ptr<Unit> hostile) {};
    virtual void OnAllyEnter(std::weak_ptr<Unit> ally) {};
    virtual void OnAllyExit(std::weak_ptr<Unit> ally) {};
    virtual void Update() override;

    std::unordered_set<std::weak_ptr<Unit>> units;
    std::unordered_set<std::weak_ptr<Unit>> foes;
    std::unordered_set<std::weak_ptr<Unit>> allies;
private:
    virtual void OnTriggerEnter(physics::Collider* other) override;
    virtual void OnTriggerExit(physics::Collider* other) override;
    bool ShouldErase(std::weak_ptr<Unit>& unit);
};
