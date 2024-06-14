#include "UnitBuffUrsula.h"
#include "InWanderLand.h"

UnitBuffUrsula::UnitBuffUrsula()
{
    durationLeft = PassiveUrsula::GetDuration();
}

void UnitBuffUrsula::OnStart()
{
    attackSpeedAdder = owner.lock()->adderAttackSpeed.AcquireFactor();
    *attackSpeedAdder = PassiveUrsula::GetAttackSpeedBonusPerStack();
    attackDamageAdder = owner.lock()->adderAttackDamage.AcquireFactor();
    *attackDamageAdder = PassiveUrsula::GetAttackDamageBonusPerStack();
};

void UnitBuffUrsula::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    if (auto sameUrsulaBuff = dynamic_cast<UnitBuffUrsula*>(&overlapping); sameUrsulaBuff)
    {
        *attackSpeedAdder = std::fmin(PassiveUrsula::GetAttackSpeedBonusPerStack() * PassiveUrsula::pod.maxStack, *attackSpeedAdder + PassiveUrsula::GetAttackSpeedBonusPerStack());
        *attackDamageAdder = std::fmin(PassiveUrsula::GetAttackDamageBonusPerStack() * PassiveUrsula::pod.maxStack, *attackDamageAdder + PassiveUrsula::GetAttackDamageBonusPerStack());
    }
}
