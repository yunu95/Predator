#include "PassiveHanselHeal.h"
#include "InWanderLand.h"
#include "VFXAnimator.h"

POD_PassiveHanselHeal PassiveHanselHeal::pod;

coroutine::Coroutine PassiveHanselHeal::CookieLingering(Vector3d pos, std::weak_ptr<Unit> owner)
{
    auto persistance = owner.lock();

	auto cookieMesh = FBXPool::Instance().Borrow("VFX_HealPack");

    auto collider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner);
    collider.lock()->SetRadius(pod.cookieRadius);
    cookieMesh.lock()->GetTransform()->SetLocalScale(pod.cookieScale * Vector3d::one);
    
    cookieContainer.insert(cookieMesh);
    colliderContainer.insert(collider);

	auto cookieEffectAnimator = cookieMesh.lock()->AcquireVFXAnimator();
    cookieEffectAnimator.lock()->SetLoop(true);
	cookieEffectAnimator.lock()->SetAutoActiveFalse();
	cookieEffectAnimator.lock()->Init();
	cookieEffectAnimator.lock()->Play();

    float offsetX = math::Random::GetRandomFloat(pod.dropRadius);
    float offsetZ = math::Random::GetRandomFloat(pod.dropRadius);

    Vector3d targetPos = pos + Vector3d(offsetX, 0, offsetZ);
    targetPos = SingleNavigationField::Instance().GetClosestPointOnField(targetPos);

    Vector3d startPos = pos + Vector3d(0, pod.maxHeight, 0);

    auto velocity = wanderUtils::GetInitSpeedOfFreeFall(pod.cakeOnAirDuration, startPos, targetPos);

    coroutine::ForSeconds forCakeFlySeconds{ pod.cakeOnAirDuration };

    auto& gc = GlobalConstant::GetSingletonInstance().pod;

    cookieMesh.lock()->GetTransform()->SetWorldPosition(startPos);
    collider.lock()->GetTransform()->SetWorldPosition(startPos);

    while (forCakeFlySeconds.Tick())
    {
        velocity += Vector3d::down * gc.gravitySpeed * Time::GetDeltaTime();
        cookieMesh.lock()->GetTransform()->SetWorldPosition(cookieMesh.lock()->GetTransform()->GetWorldPosition() + velocity * Time::GetDeltaTime());
        collider.lock()->GetTransform()->SetWorldPosition(collider.lock()->GetTransform()->GetWorldPosition() + velocity * Time::GetDeltaTime());

        auto tempPos = cookieMesh.lock()->GetTransform()->GetWorldPosition();
        if (tempPos.y <= 0)
        {
            tempPos.y = 0;
            cookieMesh.lock()->GetTransform()->SetWorldPosition(tempPos);
            collider.lock()->GetTransform()->SetWorldPosition(tempPos);
            break;
        }

        co_await std::suspend_always{};
    }

    SFXManager::PlaySoundfile("sounds/Hansel/Hansel passive.wav");

    coroutine::ForSeconds forSeconds{ pod.cookieLifetime };
    while (forSeconds.Tick())
    {
        while (isPaused)
        {
            co_await std::suspend_always{};
        }

        for (auto unit : collider.lock()->GetFriends())
        {
            if (unit->GetTeamIndex() == PlayerController::playerTeamIndex && unit->GetUnitCurrentHp() < unit->GetUnitTemplateData().pod.max_Health)
            {
                unit->Heal(PassiveHanselHeal::GetHealAmount());
                SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::Interaction_Hansel_Passive_Cake), unit->GetTransform()->GetWorldPosition());
                FBXPool::Instance().Return(cookieMesh);
                UnitAcquisitionSphereColliderPool::Instance().Return(collider);
                cookieContainer.erase(cookieMesh);
                colliderContainer.erase(collider);
                co_return;
            }
        }
        co_await std::suspend_always{};
    }
    FBXPool::Instance().Return(cookieMesh);
    UnitAcquisitionSphereColliderPool::Instance().Return(collider);
    cookieContainer.erase(cookieMesh);
    colliderContainer.erase(collider);

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

void PassiveHanselHeal::OnPause()
{
    isPaused = true;
}

void PassiveHanselHeal::OnResume()
{
    isPaused = false;
}

void PassiveHanselHeal::Recovery()
{
    // 활성화된 케이크들을 모두 없애는 로직
    for (auto each : cookieContainer)
    {
        FBXPool::Instance().Return(each);
    }
    for (auto each : colliderContainer)
    {
        UnitAcquisitionSphereColliderPool::Instance().Return(each);
    }

    cookieContainer.clear();
    colliderContainer.clear();
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
