#include "StatusEffectTrigger.h"
#include "Unit.h"

void StatusEffectTrigger::OnTriggerEnter(physics::Collider* collider)
{
    // Request StatusTimer To TimerPool here
    if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->GetUnitSide() != m_ownerUnit->GetUnitSide())
    {
        ApplyStatus(m_ownerUnit, colliderUnitComponent);
        colliderUnitComponent->ReportStatusEffectApplied(m_statusEffectType);
    }
}
