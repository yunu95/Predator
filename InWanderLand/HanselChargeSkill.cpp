#include "InWanderLand.h"
#include "HanselChargeSkill.h"

#define _USE_MATH_DEFINES
#include <math.h>

coroutine::Coroutine HanselChargeSkill::operator()()
{
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;

    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    stompCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    stompCollider.lock()->SetRadius(gc.hanselQSkillStompRadius);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    float totalTime = static_cast<float>(deltaPos.Magnitude()) / gc.hanselQSkillRushSpeed;
    coroutine::ForSeconds forSeconds{ totalTime };

    ///
    /// Animation 을 totalTime 이 되었을 때 내려 찍는 프레임이 되도록
    /// PlaySpeed 를 적당히 설정하는 로직
    /// ex) 내려찍는 애니메이션이 20 Frame일 경우.
    ///
    owner.lock()->PlayAnimation(UnitAnimType::Skill1, false);
    auto animator = owner.lock()->GetAnimator();

    float stompTimingFrame = 10.0f;
    bool isAnimationOncePaused = false;
    bool isAnimationOnceResumed = false;
    float yPos = 0.0f;

    while (forSeconds.Tick())
    {
        float temp = animator.lock()->GetCurrentFrame();
        if (stompTimingFrame <= animator.lock()->GetCurrentFrame() && !isAnimationOncePaused)
        {
            animator.lock()->Pause();
            isAnimationOncePaused = true;
        }
        if (forSeconds.ElapsedNormalized() >= 0.5f && !isAnimationOnceResumed && isAnimationOncePaused)
        {
            animator.lock()->Resume();
            isAnimationOnceResumed = true;
        }
        stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        currentPos += direction * gc.hanselQSkillRushSpeed * Time::GetDeltaTime();
        yPos = gc.hanselQSkillMaxJumpDistance * std::sin(M_PI * forSeconds.ElapsedNormalized());
        currentPos = Vector3d(currentPos.x, yPos, currentPos.z);
        owner.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
    }

    stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    co_await std::suspend_always{};

    for (auto each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, gc.hanselQSkillStompDamage);
    }

    ///
    /// Animation 의 PlaySpeed 를 원래대로 회복하는 로직
    ///
    
    while (!animator.lock()->IsDone())
    {
        co_await std::suspend_always{};
    }

    for (auto each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, gc.hanselQSkillStompDamage);
    }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(currentPos);

    OnInterruption();
    co_return;
}

void HanselChargeSkill::OnInterruption()
{
    stompCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(stompCollider);
}
