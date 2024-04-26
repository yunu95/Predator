#include "DamageOnlyComponent.h"
#include "DebuggingMeshPool.h"
#include "Unit.h"

void DamageOnlyComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	if (ownerUnit->GetUnitType() == Unit::UnitType::ChessTrap || ownerUnit->GetUnitType() == Unit::UnitType::SpikeTrap)
	{
		opponentUnit->Damaged(m_skillDamage);
	}
	else
	{
		opponentUnit->Damaged(ownerUnit, ownerUnit->DetermineAttackDamage(m_skillDamage));
	}
}

void DamageOnlyComponent::SetSkillDamage(float dmg)
{
	m_skillDamage = dmg;
}
