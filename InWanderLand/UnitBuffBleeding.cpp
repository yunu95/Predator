#include "UnitBuffBleeding.h"
#include "YunutyEngine.h"
#include "Unit.h"
#include "InWanderLand.h"

void UnitBuffBleeding::OnStart()
{
    buffEffect = FBXPool::Instance().Borrow("VFX_DeBuff_Bleeding");
    buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());

    buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();
}

void UnitBuffBleeding::OnUpdate() 
{
    if (!isPaused)
    {
        owner.lock()->Damaged(Inflictor, damagePerSecond * yunutyEngine::Time::GetDeltaTime());
    }
    if (!buffEffect.expired())
    {
        buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    }
}

void UnitBuffBleeding::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffBleeding::OnPause()
{
    isPaused = true;
}

void UnitBuffBleeding::OnResume()
{
    isPaused = false;
}

void UnitBuffBleeding::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    if (auto sameBleeding = dynamic_cast<UnitBuffBleeding*>(&overlapping); sameBleeding)
    {
        damagePerSecond = std::fmin(PassiveRobinBleed::pod.dpsPerStack * PassiveRobinBleed::pod.maxStack, damagePerSecond + sameBleeding->damagePerSecond);
    }
};
