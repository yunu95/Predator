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

    buffEffect = FBXPool::Instance().Borrow("VFX_Buff_Ursula_Contract");
    buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());

    auto buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();
}

void UnitBuffUrsula::OnUpdate()
{
    if (!buffEffect.expired())
    {
        buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
        buffEffect.lock()->GetTransform()->SetWorldScale(ownerUnitActualScale);
    }
}

void UnitBuffUrsula::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffUrsula::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    if (auto sameUrsulaBuff = dynamic_cast<UnitBuffUrsula*>(&overlapping); sameUrsulaBuff)
    {
        *attackSpeedAdder = std::fmin(PassiveUrsula::GetAttackSpeedBonusPerStack() * PassiveUrsula::pod.maxStack, *attackSpeedAdder + PassiveUrsula::GetAttackSpeedBonusPerStack());
        *critChanceAdder = std::fmin(PassiveUrsula::GetCritChanceBonusPerStack() * PassiveUrsula::pod.maxStack, *critChanceAdder + PassiveUrsula::GetCritChanceBonusPerStack());
    }
}

