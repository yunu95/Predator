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
    owner.lock()->StartCoroutine(EffectCoroutine());
}

void UnitBuffHanselDebuff::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffHanselDebuff::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    FBXPool::Instance().Return(buffEffect);
    owner.lock()->StartCoroutine(EffectCoroutine());
}

void UnitBuffHanselDebuff::OnPause()
{
    buffEffectAnimator.lock()->Pause();
}

void UnitBuffHanselDebuff::OnResume()
{
    buffEffectAnimator.lock()->Resume();
}

coroutine::Coroutine UnitBuffHanselDebuff::EffectCoroutine()
{
    buffEffect = FBXPool::Instance().Borrow("VFX_DeBuff_Blind_Hansel");
    buffEffect.lock()->GetGameObject()->SetParent(owner.lock()->GetGameObject());

    buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();

    co_return;
}
