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
    
    //    blindReference = owner.lock()->referenceBlindness.Acquire();


    buffEffect = FBXPool::Instance().Borrow("VFX_DeBuff_Blind_Hansel");
    buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());

    auto buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();
}

void UnitBuffHanselDebuff::OnUpdate()
{
    if (!buffEffect.expired())
    {
        buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
        buffEffect.lock()->GetTransform()->SetWorldScale(ownerUnitActualScale);
    }
}

void UnitBuffHanselDebuff::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffHanselDebuff::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
}

void UnitBuffHanselDebuff::OnPause()
{
}

void UnitBuffHanselDebuff::OnResume()
{
}

