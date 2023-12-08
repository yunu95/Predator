#pragma once
#include "YunutyEngine.h"
/// <summary>
/// GetGameObject->GetComponent<IRangeAction>을 통해 함수호출 할 예정. 
/// </summary>

class Unit;

class RangeSystem : public CircleCollider2D
{
private:
    Unit* m_unitComponent;
    float m_tempIDRadius;
    float m_tempAtkRadius;

public:   
    void SetOwnerUnitComponent(Unit* unitComponent);

public:
    virtual void Start() override;
    virtual void OnCollisionEnter2D(const Collision2D& collision) override;
    virtual void OnCollisionExit2D(const Collision2D& collision) override;
};

