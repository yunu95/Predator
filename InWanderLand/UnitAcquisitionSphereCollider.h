#pragma once
#include "YunutyEngine.h"

class Unit;
class UnitAcquisitionSphereCollider : public physics::SphereCollider
{
public:
    // nullable
    std::weak_ptr<Unit> owner;
    // owner 기준 적, 아군 유닛들의 목록
    const std::unordered_set<std::weak_ptr<Unit>>& GetFoes() { return foes; }
    const std::unordered_set<std::weak_ptr<Unit>>& GetFriends() { return allies; }
private:
    virtual void OnTriggerEnter(physics::Collider* other) override;
    virtual void OnTriggerExit(physics::Collider* other) override;

    std::unordered_set<std::weak_ptr<Unit>> foes;
    std::unordered_set<std::weak_ptr<Unit>> allies;
};

