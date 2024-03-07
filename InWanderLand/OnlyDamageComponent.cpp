#include "InWanderLand.h"
#include "OnlyDamageComponent.h"
#include "Unit.h"
#include "DebuggingMeshPool.h"

void OnlyDamageComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	opponentUnit->Damaged(ownerUnit, ownerUnit->DetermineAttackDamage(m_skillDamage));
	/// 도발 기능 추가
	opponentUnit->ChangeCurrentOpponentUnitForced(ownerUnit);
	auto debuggingMesh = DebuggingMeshPool::SingleInstance().Borrow();
	debuggingMesh->SetUnitObject(opponentUnit);
	debuggingMesh->PopMeshUP(yunuGI::Color::green(), MaterialNum::Green);
}

void OnlyDamageComponent::SetSkillDamage(float dmg)
{
	m_skillDamage = dmg;
}

