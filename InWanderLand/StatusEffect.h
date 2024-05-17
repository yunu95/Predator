#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"

/// <summary>
/// 원하는 때에 자식으로부터 재정의된 ApplyStatus 함수를 호출하여 사용한다.
/// </summary>

class Unit;

class StatusEffect : public Component, public ContentsObservee
{
private:
    std::weak_ptr<physics::SphereCollider> triggerCollider;
public:
    enum class StatusEffectEnum
    {
        Bleeding,
        Blinding,
        Paralysis,
        KnockBack,
        Taunted
    };

protected:
    Unit* m_ownerUnit;
    StatusEffectEnum m_statusEffectType;

    friend class DualCastComponent;

public:
    std::weak_ptr<physics::SphereCollider> GetTriggerCollider();
    virtual void Start() override;

    virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) = 0;
    virtual void SetSkillOwnerUnit(Unit* p_unit);
    virtual Component* GetComponent() override { return this; }
};

