#include "InWanderLand.h"
#include "BossSpinAttackSkill.h"
#include "VFXAnimator.h"

POD_BossSpinAttackSkill BossSpinAttackSkill::pod = POD_BossSpinAttackSkill();

const float totalTime = 5.2f;
const float spinEndTime = 3.15f;
const float spinStartTime = 2.07f;
const float spinAttackingTime = spinEndTime - spinStartTime;
const float afterSpinDelay = totalTime - spinStartTime - spinAttackingTime;

coroutine::Coroutine BossSpinAttackSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(dynamic_pointer_cast<BossSpinAttackSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            FBXPool::Instance().Return(chargeEffect);
            UnitAcquisitionSphereColliderPool::Instance().Return(knockbackCollider);
        });
    co_yield coroutine::WaitForSeconds(spinStartTime);

    coroutine::ForSeconds forSeconds{ spinAttackingTime };

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    co_yield coroutine::WaitForSeconds(afterSpinDelay);
    owner.lock()->PlayAnimation(UnitAnimType::Idle, true);
    co_yield coroutine::WaitForSeconds(0.2);
    co_return;
}

void BossSpinAttackSkill::OnInterruption()
{
    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
}

coroutine::Coroutine BossSpinAttackSkill::SpawningSkillffect(std::weak_ptr<BossSpinAttackSkill> skill)
{
    colliderEffectRatio = 10.0f * 0.5f;

    float actualCollideRange = pod.skillRadius * 1 / (colliderEffectRatio);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();


	chargeEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Skill1");

    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().x,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().y,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().z));
    auto chargeEffectAnimator = chargeEffect.lock()->AcquireVFXAnimator();
    chargeEffectAnimator.lock()->SetAutoActiveFalse();
    chargeEffectAnimator.lock()->Init();

    knockbackCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    knockbackCollider.lock()->SetRadius(pod.skillRadius * owner.lock()->GetTransform()->GetWorldScale().x);
    knockbackCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    std::unordered_set<Unit*> knockBackList;

    co_yield coroutine::WaitForSeconds(spinStartTime);

    coroutine::ForSeconds forSeconds{ spinAttackingTime };
    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};
        for (auto& each : knockbackCollider.lock()->GetEnemies())
        {
            if (knockBackList.contains(each))
            {
                continue;
            }
            knockBackList.insert(each);
            Vector3d delta = pod.knockBackDistance * (each->GetTransform()->GetWorldPosition() - owner.lock()->GetTransform()->GetWorldPosition()).Normalized();
            each->Damaged(owner, pod.skillDamage);
            each->Paralyze(pod.paralyzingDuration);
            each->KnockBack(owner.lock()->GetTransform()->GetWorldPosition() + delta, pod.knockBackDuration);
        }
    }

    co_await std::suspend_always{};

    float elapsed = 0.0f;

    while (elapsed <= pod.skillEndTimeAfterDamaged)
    {
        elapsed += Time::GetDeltaTime();
        co_await std::suspend_always{};
    }


    co_return;
}
