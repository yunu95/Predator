#include "InWanderLand.h"
#include "SpecialEffect.h"
#include "Unit.h"

//void SpecialEffect::OnTriggerEnter(physics::Collider* collider)
//{
//	// Request StatusTimer To TimerPool here
//	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
//		colliderUnitComponent != nullptr &&
//		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Enemy)
//	{
//		ApplyStatus(m_ownerUnit, colliderUnitComponent);
//	}
//}

void SpecialEffect::SetSkillOwnerUnit(Unit* p_unit)
{
	m_ownerUnit = p_unit;
}

