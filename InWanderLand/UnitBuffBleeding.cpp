#include "UnitBuffBleeding.h"
#include "YunutyEngine.h"
#include "Unit.h"
#include "InWanderLand.h"

coroutine::Coroutine UnitBuffBleeding::EffectCoroutine()
{
    buffEffect = FBXPool::Instance().Borrow("VFX_DeBuff_Bleeding");
    buffEffect.lock()->GetGameObject()->SetParent(owner.lock()->GetGameObject());

    buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();

    co_return;
}

void UnitBuffBleeding::OnStart()
{
    owner.lock()->StartCoroutine(EffectCoroutine());
}

void UnitBuffBleeding::OnUpdate() 
{
    if (!isPaused)
    {
        owner.lock()->Damaged(Inflictor, damagePerSecond * yunutyEngine::Time::GetDeltaTime());
    }
}

void UnitBuffBleeding::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffBleeding::OnPause()
{
    isPaused = true;
    buffEffectAnimator.lock()->Pause();
}

void UnitBuffBleeding::OnResume()
{
    isPaused = false;
    buffEffectAnimator.lock()->Resume();
}

void UnitBuffBleeding::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    if (auto sameBleeding = dynamic_cast<UnitBuffBleeding*>(&overlapping); sameBleeding)
    {
        damagePerSecond = std::fmin(PassiveRobinBleed::pod.dpsPerStack * PassiveRobinBleed::pod.maxStack, damagePerSecond + sameBleeding->damagePerSecond);
    }
    FBXPool::Instance().Return(buffEffect);
    owner.lock()->StartCoroutine(EffectCoroutine());
};
