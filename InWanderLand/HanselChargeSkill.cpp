#include "InWanderLand.h"
#include "HanselChargeSkill.h"

POD_HanselChargeSkill HanselChargeSkill::pod = POD_HanselChargeSkill();

#define _USE_MATH_DEFINES
#include <math.h>

const float stompTimingFrame = 91.0f;
const float endFrame = 145.0f;
const float	duration = 0.7f;
const float damageDelay = 1.5f;
const float jumpTiming = 1.05f;

coroutine::Coroutine HanselChargeSkill::operator()()
{
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill1);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, false);
    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(dynamic_pointer_cast<HanselChargeSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            UnitAcquisitionSphereColliderPool::Instance().Return(stompCollider);
        });

    co_yield coroutine::WaitForSeconds(anim->GetDuration());

    co_return;
}

void HanselChargeSkill::OnInterruption()
{
    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
}

coroutine::Coroutine HanselChargeSkill::SpawningFieldEffect(std::weak_ptr<HanselChargeSkill> skill)
{
    auto persistance = skill.lock();
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();

    stompCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    stompCollider.lock()->SetRadius(pod.skillRadius);
    stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    auto animator = owner.lock()->GetAnimator();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill1);

    co_yield coroutine::WaitForSeconds(jumpTiming);

    float rushSpeed = static_cast<float>(deltaPos.Magnitude()) / duration;
    coroutine::ForSeconds forSeconds{ duration };

    // y = vy0 * t - 0.5 * a * t^2
    // y가 0일 때, t는 Duration이고, t = Duration / 2 일 때, y는 jumpDistance.
    // 연립방정식 풀면,
    // a = (8 * jumpDistance) / Duration^2
    // vy0 = 4 * jumpDistance / Duration

    float vy0 = 4 * pod.maxJumpHeight / duration;
    float acc = (8 * pod.maxJumpHeight) / (duration * duration);
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

    /*while (stompTimingFrame >= animator.lock()->GetCurrentFrame())
    {
        co_await std::suspend_always{};
    }*/

    co_yield coroutine::WaitForSeconds(damageDelay);

    for (auto each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, pod.damage);
    }

    co_yield coroutine::WaitForSeconds(anim->GetDuration() - jumpTiming - duration - damageDelay);

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(currentPos);

    co_return;
}
