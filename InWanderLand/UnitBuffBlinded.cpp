#include "UnitBuffBlinded.h"
#include "InWanderLand.h"

UnitBuffBlinded::UnitBuffBlinded()
{
    durationLeft = UrsulaBlindSkill::pod.skillBlindTime;
}

UnitBuffType UnitBuffBlinded::GetBuffType()
{
    return UnitBuffType::Blinded;
}

UIEnumID UnitBuffBlinded::GetUIEnumID()
{
    return UIEnumID::CharInfo_Buff_Blinding;
}

void UnitBuffBlinded::OnStart()
{
    blindReference = owner.lock()->referenceBlindness.Acquire();
    owner.lock()->StartCoroutine(EffectCoroutine());
}

void UnitBuffBlinded::OnUpdate()
{
    if (!buffEffect.expired())
    {
        buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    }
}

void UnitBuffBlinded::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffBlinded::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    FBXPool::Instance().Return(buffEffect);
    owner.lock()->StartCoroutine(EffectCoroutine());
}

void UnitBuffBlinded::OnPause()
{
}

void UnitBuffBlinded::OnResume()
{
}

coroutine::Coroutine UnitBuffBlinded::EffectCoroutine()
{
    buffEffect = FBXPool::Instance().Borrow("VFX_DeBuff_Blind_Ursula");
    buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());

    buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();

    co_return;
}
