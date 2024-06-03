#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"
#include "UnitAcquisitionSphereCollider.h"

class Unit;

class CursorDetector : public UnitAcquisitionSphereCollider, public ContentsObservee
{
public:
    virtual void OnUnitEnter(Unit* hostile) override;
    virtual void OnUnitExit(Unit* hostile) override;
    virtual void OnFriendEnter(Unit* hostile) override;
    virtual void OnFriendExit(Unit* hostile) override;
    virtual void OnEnemyEnter(Unit* hostile) override;
    virtual void OnEnemyExit(Unit* hostile) override;

    virtual void Start() override;
    virtual void Update() override;

    std::weak_ptr<Unit> GetCurrentOnMouseUnit() const;
    virtual Component* GetComponent() override { return this; }
private:
    GameObject* m_SelectEffectObject;
};
