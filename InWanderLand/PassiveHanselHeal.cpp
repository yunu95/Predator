#include "PassiveHanselHeal.h"
#include "InWanderLand.h"

POD_PassiveHanselHeal PassiveHanselHeal::pod;
std::weak_ptr<ManagedFBX> PassiveHanselHeal::borrowedFBX = std::weak_ptr<ManagedFBX>();
std::weak_ptr<UnitAcquisitionSphereCollider> PassiveHanselHeal::borrowedCollider = std::weak_ptr<UnitAcquisitionSphereCollider>();
coroutine::Coroutine PassiveHanselHeal::CookieLingering(Vector3d pos, std::weak_ptr<Unit> owner)
{
    pos = SingleNavigationField::Instance().GetClosestPointOnField(pos);
    borrowedFBX = FBXPool::Instance().Borrow(wanderResources::GetFBXName(wanderResources::WanderFBX::HEALING_COOKIE));
    borrowedCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner);
    borrowedCollider.lock()->SetRadius(pod.cookieRadius);
    borrowedFBX.lock()->GetTransform()->SetWorldPosition(pos);
    borrowedFBX.lock()->GetTransform()->SetLocalScale(pod.cookieScale * Vector3d::one);
    coroutine::ForSeconds forSeconds{ pod.cookieLifetime };
    while (forSeconds.Tick())
    {
        for (auto unit : borrowedCollider.lock()->GetFriends())
        {
            if (unit->GetTeamIndex() == PlayerController::playerTeamIndex && unit->GetUnitCurrentHp() < unit->GetUnitTemplateData().pod.max_Health)
            {
                unit->Heal(pod.healAmount);
                FBXPool::Instance().Return(borrowedFBX);
                co_return;
            }
        }
        borrowedFBX.lock()->GetTransform()->SetWorldPosition(pos);
        co_await std::suspend_always{};
    }
    FBXPool::Instance().Return(borrowedFBX);
    co_return;
}

void PassiveHanselHeal::Init(std::weak_ptr<Unit> owner)
{
    owner.lock()->onAttack.AddCallback([this](std::weak_ptr<Unit>)
        {
            IncrementHitCounter();
        });
}

void PassiveHanselHeal::IncrementHitCounter()
{
    hitCounter++;
    if (hitCounter >= pod.hitsRequired)
    {
        auto coro = ContentsCoroutine::StartRoutine(CookieLingering(owner.lock()->GetTransform()->GetWorldPosition(), owner));
        coro.lock()->PushDestroyCallBack([]() 
            {
                FBXPool::Instance().Return(borrowedFBX);
                UnitAcquisitionSphereColliderPool::Instance().Return(borrowedCollider);
            });
        hitCounter = 0;
    };
}
