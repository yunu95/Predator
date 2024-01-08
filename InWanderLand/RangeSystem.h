#pragma once
#include "YunutyEngine.h"
/// <summary>
/// GetGameObject->GetComponent<IRangeAction>을 통해 함수호출 할 예정. 
/// </summary>

class Unit;

class RangeSystem : public Component
{
private:
    Unit* m_unitComponent;
    float m_tempIDRadius;
    float m_tempAtkRadius;

public:   
    void SetOwnerUnitComponent(Unit* unitComponent);

public:
    virtual void Start() override;
    virtual void OnTriggerEnter(physics::Collider* collider) override;
    virtual void OnTriggerExit(physics::Collider* collider) override;
};

