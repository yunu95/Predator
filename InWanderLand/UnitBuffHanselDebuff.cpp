#include "UnitBuffHanselDebuff.h"
#include "InWanderLand.h"

UnitBuffHanselDebuff::UnitBuffHanselDebuff()
{
    durationLeft = HanselProjectileSkill::pod.debuffDuration;
}

void UnitBuffHanselDebuff::OnStart()
{
    attackSpeedAdder = owner.lock()->adderAttackSpeed.AcquireFactor();
    *attackSpeedAdder = HanselProjectileSkill::pod.debuffAttackSpeedAdder;
    attackDamageMultiplier = owner.lock()->multiplierDamage.AcquireFactor();
    *attackDamageMultiplier = HanselProjectileSkill::pod.debuffAttackDamageMultiplier;
};