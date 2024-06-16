#include "UnitBuffHanselBuff.h"
#include "InWanderLand.h"

UnitBuffHanselBuff::UnitBuffHanselBuff()
{
    durationLeft = HanselProjectileSkill::pod.buffDuration;
}

void UnitBuffHanselBuff::OnStart()
{
    attackSpeedAdder = owner.lock()->adderAttackSpeed.AcquireFactor();
    *attackSpeedAdder = HanselProjectileSkill::pod.attackSpeedBonus;
    critChanceAdder = owner.lock()->adderCritChance.AcquireFactor();
    *critChanceAdder = HanselProjectileSkill::pod.critChanceBonus;
};
