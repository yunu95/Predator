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

    buffEffect = FBXPool::Instance().Borrow("VFX_Buff_Hansel_Pie");
    buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());

    buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();
}

void UnitBuffHanselBuff::OnUpdate()
{
    if (!buffEffect.expired())
    {
        buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    }
}

void UnitBuffHanselBuff::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffHanselBuff::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
}

void UnitBuffHanselBuff::OnPause()
{
}

void UnitBuffHanselBuff::OnResume()
{
}


