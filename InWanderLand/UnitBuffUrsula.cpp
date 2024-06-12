#include "UnitBuffUrsula.h"
#include "InWanderLand.h"

UnitBuffUrsula::UnitBuffUrsula()
{
    durationLeft = PassiveUrsula::pod.duration;
}

void UnitBuffUrsula::OnStart()
{
    attackSpeedAdder = owner.lock()->adderAttackSpeed.AcquireFactor();
    *attackSpeedAdder = PassiveUrsula::pod.attackSpeedBonusPerStack;
    attackDamageAdder = owner.lock()->adderAttackDamage.AcquireFactor();
    *attackDamageAdder = PassiveUrsula::pod.attackDamageBonusPerStack;
};

void UnitBuffUrsula::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    if (auto sameUrsulaBuff = dynamic_cast<UnitBuffUrsula*>(&overlapping); sameUrsulaBuff)
    {
        *attackSpeedAdder = std::fmin(PassiveUrsula::pod.attackSpeedBonusPerStack * PassiveUrsula::pod.maxStack, *attackSpeedAdder + PassiveUrsula::pod.attackSpeedBonusPerStack);
        *attackDamageAdder = std::fmin(PassiveUrsula::pod.attackDamageBonusPerStack * PassiveUrsula::pod.maxStack, *attackDamageAdder + PassiveUrsula::pod.attackDamageBonusPerStack);
    }
}
