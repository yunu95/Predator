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
    critChanceAdder = owner.lock()->adderCritChance.AcquireFactor();
    *critChanceAdder = PassiveUrsula::GetCritChanceBonusPerStack();
};

void UnitBuffUrsula::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    if (auto sameUrsulaBuff = dynamic_cast<UnitBuffUrsula*>(&overlapping); sameUrsulaBuff)
    {
        *attackSpeedAdder = std::fmin(PassiveUrsula::GetAttackSpeedBonusPerStack() * PassiveUrsula::pod.maxStack, *attackSpeedAdder + PassiveUrsula::GetAttackSpeedBonusPerStack());
        *critChanceAdder = std::fmin(PassiveUrsula::GetCritChanceBonusPerStack() * PassiveUrsula::pod.maxStack, *critChanceAdder + PassiveUrsula::GetCritChanceBonusPerStack());
    }
}
