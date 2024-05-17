#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"

/// <summary>
/// GetGameObject->GetComponent<IRangeAction>을 통해 함수호출 할 예정. 
/// </summary>

class Unit;

class RangeSystem : public ContentsObservee
{
private:
    Unit* m_unitComponent;

public:   
    void SetOwnerUnitComponent(Unit* unitComponent);

public:
    virtual void Start() override;
    virtual void OnTriggerEnter(physics::Collider* collider) override;
    virtual void OnTriggerExit(physics::Collider* collider) override;
};

