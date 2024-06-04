#include "InWanderLand.h"
#include "HanselChargeSkill.h"

coroutine::Coroutine HanselChargeSkill::operator()()
{
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;

    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    stompCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    stompCollider.lock()->SetRadius(gc.hanselQSkillStompRadius);
    stompCollider.lock()->SetActive(false);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);
    if (deltaPos.Magnitude() > gc.hanselQSkillMaxDistance)
    {
        deltaPos = direction * gc.hanselQSkillMaxDistance;
    }
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
    float totalTime = static_cast<float>(deltaPos.Magnitude()) / gc.hanselQSkillSpeed;
    coroutine::ForSeconds forSeconds{ totalTime };

    float yPos = 0.0f;
    float localTime = 0.0f;
    float ratio = 0.0f;

    while (forSeconds.Tick())
    {
        localTime += Time::GetDeltaTime();
        ratio = localTime / totalTime;
        if (ratio < 0.5)
            yPos = std::lerp(yPos, gc.hanselQSkillMaxJumpDistance, ratio);
        else
            yPos = std::lerp(yPos, 0, ratio);
    

        currentPos += direction * gc.hanselQSkillSpeed * Time::GetDeltaTime();
        currentPos = Vector3d(currentPos.x, yPos, currentPos.z);
        owner.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
    }

    coroutine::ForSeconds forStumpDelaySeconds{ gc.hanselQSkillStompDelay };

    while (forStumpDelaySeconds.Tick())
    {
        co_await std::suspend_always{};
    }

    coroutine::ForSeconds forStompRemainSeconds{ gc.hanselQSkillStompDelay };
    stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    stompCollider.lock()->SetActive(true);
    co_await std::suspend_always{};
    for (auto& each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, gc.hanselQSkillStompDamage);
    }
    stompCollider.lock()->SetActive(false);
    
    coroutine::ForSeconds forNextStumpDelaySeconds{ gc.hanselQSkillStompDelay };

    while (forNextStumpDelaySeconds.Tick())
    {
        co_await std::suspend_always{};
    }

    stompCollider.lock()->SetActive(true);
    co_await std::suspend_always{};
    for (auto& each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, gc.hanselQSkillStompDamage);
    }
    stompCollider.lock()->SetActive(false);

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(currentPos);

    co_return;
}
