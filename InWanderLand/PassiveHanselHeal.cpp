#include "PassiveHanselHeal.h"
#include "InWanderLand.h"
#include "VFXAnimator.h"

POD_PassiveHanselHeal PassiveHanselHeal::pod;
coroutine::Coroutine PassiveHanselHeal::CookieLingering(Vector3d pos, std::weak_ptr<Unit> owner)
{
    pos = SingleNavigationField::Instance().GetClosestPointOnField(pos);
	auto cookieMesh = FBXPool::Instance().Borrow("VFX_HealPack");

    auto collider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner);
    collider.lock()->SetRadius(pod.cookieRadius);
    collider.lock()->GetTransform()->SetWorldPosition(pos);
    cookieMesh.lock()->GetTransform()->SetWorldPosition(pos);
    cookieMesh.lock()->GetTransform()->SetLocalScale(pod.cookieScale * Vector3d::one);
    
	auto cookieEffectAnimator = cookieMesh.lock()->AcquireVFXAnimator();
    cookieEffectAnimator.lock()->SetLoop(true);
	cookieEffectAnimator.lock()->SetAutoActiveFalse();
	cookieEffectAnimator.lock()->Init();
	cookieEffectAnimator.lock()->Play();

    coroutine::ForSeconds forSeconds{ pod.cookieLifetime };
    while (forSeconds.Tick())
    {
        for (auto unit : collider.lock()->GetFriends())
        {
            if (unit->GetTeamIndex() == PlayerController::playerTeamIndex && unit->GetUnitCurrentHp() < unit->GetUnitTemplateData().pod.max_Health)
            {
                unit->Heal(PassiveHanselHeal::GetHealAmount());
                FBXPool::Instance().Return(cookieMesh);
                co_return;
            }
        }
        cookieMesh.lock()->GetTransform()->SetWorldPosition(pos);
        co_await std::suspend_always{};
    }
    FBXPool::Instance().Return(cookieMesh);
    co_return;
}

void PassiveHanselHeal::Init(std::weak_ptr<Unit> owner)
{
    owner.lock()->onAttack.AddCallback([this](std::weak_ptr<Unit>)
        {
            IncrementHitCounter();
        });
}

float PassiveHanselHeal::GetHealAmount()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::HANSEL_PASSIVE_ENHANCE) ? pod.healAmountUpgraded : pod.healAmount;
}

void PassiveHanselHeal::IncrementHitCounter()
{
    hitCounter++;
    if (hitCounter >= pod.hitsRequired)
    {
        ContentsCoroutine::StartRoutine(CookieLingering(owner.lock()->GetTransform()->GetWorldPosition(), owner));
        hitCounter = 0;
    };
}
