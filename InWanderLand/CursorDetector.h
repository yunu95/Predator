#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"
#include "UnitAcquisitionSphereCollider.h"

class Unit;

class CursorDetector : public UnitAcquisitionSphereCollider, public ContentsObservee
{
public:
    virtual void OnUnitEnter(std::weak_ptr<Unit> hostile) override;
    virtual void OnUnitExit(std::weak_ptr<Unit> hostile) override;
    virtual void OnAllyEnter(std::weak_ptr<Unit> hostile) override;
    virtual void OnAllyExit(std::weak_ptr<Unit> hostile) override;
    virtual void OnHostileEnter(std::weak_ptr<Unit> hostile) override;
    virtual void OnHostileExit(std::weak_ptr<Unit> hostile) override;

    virtual void Start() override;
    virtual void Update() override;

    std::weak_ptr<Unit> GetCurrentOnMouseUnit() const;
    virtual Component* GetComponent() override { return this; }
private:
    GameObject* m_SelectEffectObject;
};
