#include "UnitBuffTaunted.h"
#include "InWanderLand.h"

UnitBuffType UnitBuffTaunted::GetBuffType()
{
    return UnitBuffType::Taunted;
}

UIEnumID UnitBuffTaunted::GetUIEnumID()
{
    return UIEnumID::CharInfo_Buff_Taunted;
}

void UnitBuffTaunted::OnStart()
{
    buffEffect = FBXPool::Instance().Borrow("VFX_DeBuff_Aggro");
    buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());

    buffEffectAnimator = buffEffect.lock()->AcquireVFXAnimator();
    buffEffectAnimator.lock()->SetAutoActiveFalse();
    buffEffectAnimator.lock()->SetLoop(true);
    buffEffectAnimator.lock()->Init();
    buffEffectAnimator.lock()->Play();
}

void UnitBuffTaunted::OnEnd()
{
    FBXPool::Instance().Return(buffEffect);
}

void UnitBuffTaunted::OnUpdate()
{
    if (!blockPendingCommand.get() && owner.lock()->currentTargetUnit.lock().get() == inflictor.lock().get())
    {
        blockPendingCommand = owner.lock()->referenceBlockPendingOrder.Acquire();
    }
    if (!buffEffect.expired())
    {
        buffEffect.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        buffEffect.lock()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    }
}

void UnitBuffTaunted::OnPause()
{
}

void UnitBuffTaunted::OnResume()
{
}

void UnitBuffTaunted::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
}

UnitBuffTaunted::UnitBuffTaunted(std::weak_ptr<Unit> inflictor)
{
    durationLeft = RobinTauntSkill::pod.skillTauntTime;
    this->inflictor = inflictor;
}

