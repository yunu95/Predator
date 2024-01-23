#include "InWanderLand.h"
#include "OnlyDamageComponent.h"
#include "Unit.h"
#include "DebuggingMeshPool.h"

void OnlyDamageComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	opponentUnit->Damaged(ownerUnit->GetGameObject(), ownerUnit->DetermineAttackDamage(m_skillDamage));
	
	auto debuggingMesh = DebuggingMeshPool::SingleInstance().Borrow();
	debuggingMesh->SetUnitObject(opponentUnit);
	debuggingMesh->PopMeshUP(yunuGI::Color::green(), MaterialNum::Green);
}

void OnlyDamageComponent::SetSkillDamage(float dmg)
{
	m_skillDamage = dmg;
}

