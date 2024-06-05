#include "InWanderLand.h"
#include "HanselChargeSkill.h"

POD_HanselChargeSkill HanselChargeSkill::pod = POD_HanselChargeSkill();

#define _USE_MATH_DEFINES
#include <math.h>

const float jumpTimingFrame = 35.0f;
const float stompTimingFrame = 91.0f;
const float endFrame = 145.0f;
const float	duration = 0.7f;

float HanselChargeSkill::GetCastRange()
{
    return pod.maxRange;
}

coroutine::Coroutine HanselChargeSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    stompCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    stompCollider.lock()->SetRadius(pod.stompRadius);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, false);
    auto animator = owner.lock()->GetAnimator();

    float rushSpeed = static_cast<float>(deltaPos.Magnitude()) / duration;
    coroutine::ForSeconds forSeconds{ duration };

    // y = vy0 * t - 0.5 * a * t^2
    // y가 0일 때, t는 Duration이고, t = Duration / 2 일 때, y는 jumpDistance.
    // 연립방정식 풀면,
    // a = (8 * jumpDistance) / Duration^2
    // vy0 = 4 * jumpDistance / Duration

    float vy0 = 4 * pod.maxJumpHeight / duration;
    float acc = (8 * pod.maxJumpHeight) / (duration * duration);
    while (jumpTimingFrame >= animator.lock()->GetCurrentFrame())
    {
        co_await std::suspend_always{};
    }

    bool isAnimationOncePaused = false;
    bool isAnimationOnceResumed = false;
    float yPos = 0.0f;
    while (forSeconds.Tick())
    {
        stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        currentPos += direction * rushSpeed * Time::GetDeltaTime();
        //yPos = gc.hanselQSkillMaxJumpDistance * std::sin(M_PI * forSeconds.ElapsedNormalized());
        yPos = vy0 * forSeconds.Elapsed() - 0.5f * acc * forSeconds.Elapsed() * forSeconds.Elapsed();
        currentPos = Vector3d(currentPos.x, yPos, currentPos.z);
        owner.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
    }

    stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    co_await std::suspend_always{};

    for (auto each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, pod.damage);
    }
    
    while (stompTimingFrame >= animator.lock()->GetCurrentFrame())
    {
        co_await std::suspend_always{};
    }

    for (auto each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, pod.damage);
    }

    while (endFrame - 1 >= animator.lock()->GetCurrentFrame())
    {
        float temp = animator.lock()->GetCurrentFrame();
        co_await std::suspend_always{};
    }

    //while (!animator.lock()->IsDone())
    //{
    //    co_await std::suspend_always{};
    //}

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
