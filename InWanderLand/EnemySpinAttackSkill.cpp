#include "InWanderLand.h"
#include "EnemySpinAttackSkill.h"
#include "VFXAnimator.h"

POD_EnemySpinAttackSkill EnemySpinAttackSkill::pod = POD_EnemySpinAttackSkill();

const float eliteTotalTime = 5.2f;
const float eliteSpinEndTime = 3.15f;
const float eliteSpinStartTime = 2.07f;
const float eliteSpinAttackingTime = eliteSpinEndTime - eliteSpinStartTime;
const float eliteAfterSpinDelay = eliteTotalTime - eliteSpinStartTime - eliteSpinAttackingTime;
const float colliderEffectRatio = 10.0f * 0.5f;

coroutine::Coroutine EnemySpinAttackSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Spin, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(dynamic_pointer_cast<EnemySpinAttackSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            FBXPool::Instance().Return(chargeEffect);
            UnitAcquisitionSphereColliderPool::Instance().Return(knockbackCollider);
        });
    co_yield coroutine::WaitForSeconds(eliteSpinStartTime);

    coroutine::ForSeconds forSeconds{ eliteSpinAttackingTime };

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    co_yield coroutine::WaitForSeconds(eliteAfterSpinDelay);
    owner.lock()->PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
    co_yield coroutine::WaitForSeconds(0.2);
    co_return;
}

void EnemySpinAttackSkill::OnInterruption()
{
    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
}

coroutine::Coroutine EnemySpinAttackSkill::SpawningSkillffect(std::weak_ptr<EnemySpinAttackSkill> skill)
{
    float actualCollideRange = pod.skillRadius * (1 / colliderEffectRatio);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    chargeEffect = FBXPool::Instance().Borrow("VFX_Monster1_Skill");

    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().x,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().y,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().z));
    auto chargeEffectAnimator = chargeEffect.lock()->AcquireVFXAnimator();
    chargeEffectAnimator.lock()->SetAutoActiveFalse();
    chargeEffectAnimator.lock()->Init();
    chargeEffectAnimator.lock()->Play();

    knockbackCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    knockbackCollider.lock()->SetRadius(pod.skillRadius * owner.lock()->GetTransform()->GetWorldScale().x);
    knockbackCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    std::unordered_set<Unit*> knockBackList;

    co_yield coroutine::WaitForSeconds(eliteSpinStartTime);

    coroutine::ForSeconds forSeconds{ eliteSpinAttackingTime };
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