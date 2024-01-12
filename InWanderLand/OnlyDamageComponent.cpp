#include "InWanderLand.h"
#include "OnlyDamageComponent.h"
#include "Unit.h"
#include "UnitTransformComponent.h"

void OnlyDamageComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	opponentUnit->Damaged(skillDamage);
}

void OnlyDamageComponent::SetSkillDamage(int dmg)
{
	skillDamage = dmg;
}

