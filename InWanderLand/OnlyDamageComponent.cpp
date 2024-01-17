#include "OnlyDamageComponent.h"
#include "Unit.h"
#include "UnitTransformComponent.h"

void OnlyDamageComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	opponentUnit->Damaged(ownerUnit->GetGameObject(), ownerUnit->DetermineAttackDamage(m_skillDamage));
}

void OnlyDamageComponent::SetSkillDamage(float dmg)
{
	m_skillDamage = dmg;
}

