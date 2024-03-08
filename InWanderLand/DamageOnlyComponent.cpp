#include "DamageOnlyComponent.h"
#include "DebuggingMeshPool.h"
#include "Unit.h"

void DamageOnlyComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	opponentUnit->Damaged(ownerUnit, ownerUnit->DetermineAttackDamage(m_skillDamage));

	auto debuggingMesh = DebuggingMeshPool::SingleInstance().Borrow();
	debuggingMesh->SetUnitObject(opponentUnit);
	debuggingMesh->PopMeshUP(yunuGI::Color::green(), MaterialNum::Green);
}

void DamageOnlyComponent::SetSkillDamage(float dmg)
{
	m_skillDamage = dmg;
}
