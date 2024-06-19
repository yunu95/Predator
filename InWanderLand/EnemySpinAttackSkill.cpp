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
    auto rotRef = owner.lock()->referenceBlockRotation.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Spin, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(dynamic_pointer_cast<EnemySpinAttackSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            FBXPool::Instance().Return(chargeEffect);
            FBXPool::Instance().Return(previewEffect);
            UnitAcquisitionSphereColliderPool::Instance().Return(knockbackCollider);
        });

    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitSpinStart{ owner, eliteSpinStartTime };

    while (waitSpinStart.Tick())
    {
        co_await std::suspend_always();
    }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());

    wanderUtils::UnitCoroutine::ForSecondsFromUnit afterSpinDelayTimer{ owner, eliteAfterSpinDelay };

    while (afterSpinDelayTimer.Tick())
    {
        co_await std::suspend_always();
    }

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

void EnemySpinAttackSkill::OnPause()
{
    if (!chargeEffectAnimator.expired())
    {
        chargeEffectAnimator.lock()->Pause();
    }

    if (!previewEffectAnimator.expired())
    {
        previewEffectAnimator.lock()->Pause();
    }
}

void EnemySpinAttackSkill::OnResume()
{
    if (!chargeEffectAnimator.expired())
    {
        chargeEffectAnimator.lock()->Resume();
    }

    if (!previewEffectAnimator.expired())
    {
        previewEffectAnimator.lock()->Resume();
    }
}

coroutine::Coroutine EnemySpinAttackSkill::SpawningSkillffect(std::weak_ptr<EnemySpinAttackSkill> skill)
{
    float actualCollideRange = pod.skillRadius * (1 / colliderEffectRatio);

    auto persistance = skill.lock();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    previewEffect = FBXPool::Instance().Borrow("VFX_Monster1_Skill_Preview");
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().x,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().y,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().z));
    previewEffectAnimator = previewEffect.lock()->AcquireVFXAnimator();
    previewEffectAnimator.lock()->SetAutoActiveFalse();
    previewEffectAnimator.lock()->Init();
    previewEffectAnimator.lock()->Play();


    chargeEffect = FBXPool::Instance().Borrow("VFX_Monster1_Skill");
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().x,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().y,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().z));
    chargeEffectAnimator = chargeEffect.lock()->AcquireVFXAnimator();
    chargeEffectAnimator.lock()->SetAutoActiveFalse();
    chargeEffectAnimator.lock()->Init();
    chargeEffectAnimator.lock()->Play();

    knockbackCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    knockbackCollider.lock()->SetRadius(pod.skillRadius * owner.lock()->GetTransform()->GetWorldScale().x);
    knockbackCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    std::unordered_set<Unit*> knockBackList;

    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitSpinStart{ skill.lock()->owner, eliteSpinStartTime };

    while (waitSpinStart.Tick())
    {
        co_await std::suspend_always();
    }

    wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ skill.lock()->owner, eliteSpinAttackingTime };
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

    wanderUtils::UnitCoroutine::ForSecondsFromUnit afterDamagedDelay{ skill.lock()->owner, pod.skillEndTimeAfterDamaged };

    while (afterDamagedDelay.Tick())
    {
        co_await std::suspend_always();
    }

    co_return;
}