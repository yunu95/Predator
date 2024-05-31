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
    virtual void OnAllyEnter(Unit* hostile) override;
    virtual void OnAllyExit(Unit* hostile) override;
    virtual void OnHostileEnter(Unit* hostile) override;
    virtual void OnHostileExit(Unit* hostile) override;

    virtual void Start() override;
    virtual void Update() override;

    std::weak_ptr<Unit> GetCurrentOnMouseUnit() const;
    virtual Component* GetComponent() override { return this; }
private:
    GameObject* m_SelectEffectObject;
};
