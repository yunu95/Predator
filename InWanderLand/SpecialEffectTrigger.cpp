#include "SpecialEffectTrigger.h"
#include "Unit.h"

void SpecialEffectTrigger::OnTriggerEnter(physics::Collider* collider)
{
	// Request StatusTimer To TimerPool here
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() != m_ownerUnit->GetUnitSide())
	{
		ApplyStatus(m_ownerUnit, colliderUnitComponent);
	}
}
