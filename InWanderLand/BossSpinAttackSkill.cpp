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
    auto rotRef = owner.lock()->referenceBlockRotation.Acquire();
    animator = owner.lock()->GetAnimator();

    owner.lock()->SetDefaultAnimation(UnitAnimType::None);
    owner.lock()->PlayAnimation(UnitAnimType::Skill1, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);

    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(dynamic_pointer_cast<BossSpinAttackSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            animator.lock()->GetGI().SetPlaySpeed(1);
            previewEffectAnimator.lock()->SetSpeed(1);
            chargeEffectAnimator.lock()->SetSpeed(1);
            FBXPool::Instance().Return(chargeEffect);
            FBXPool::Instance().Return(previewEffect);
            UnitAcquisitionSphereColliderPool::Instance().Return(knockbackCollider);
        });

    float localForeswingDuration;
    if (pod.foreswingDuration > 0)
    {
        localForeswingDuration = pod.foreswingDuration;
    }
    else
    {
        localForeswingDuration = spinStartTime;
    }
    float localSkillDuration;
    if (pod.skillDuration > 0)
    {
        localSkillDuration = pod.skillDuration;
    }
    else
    {
        localSkillDuration = spinAttackingTime;
    }
    float localBackswingDuration;
    if (pod.backswingDuration > 0)
    {
        localBackswingDuration = pod.backswingDuration;
    }
    else
    {
        localBackswingDuration = wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Spin)->GetDuration() - spinStartTime - spinAttackingTime;
    }
    wanderUtils::UnitCoroutine::ForSecondsFromUnit spinTimer{ owner, localForeswingDuration + localSkillDuration + localBackswingDuration };

    while (spinTimer.Tick())
    {
        co_await std::suspend_always();
    }
    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
        owner.lock()->OrderAttackMove();
    animator.lock()->GetGI().SetPlaySpeed(1);
    co_return;
}

void BossSpinAttackSkill::OnInterruption()
{
    animator.lock()->GetGI().SetPlaySpeed(1);

    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
}

void BossSpinAttackSkill::OnPause()
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

void BossSpinAttackSkill::OnResume()
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

coroutine::Coroutine BossSpinAttackSkill::SpawningSkillffect(std::weak_ptr<BossSpinAttackSkill> skill)
{
    auto persistance = skill.lock();

    colliderEffectRatio = 10.0f * 0.7f;

    float actualCollideRange = pod.skillRadius * 1 / (colliderEffectRatio);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    previewEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Skill1_Preview");
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().x,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().y,
        actualCollideRange * owner.lock()->GetTransform()->GetWorldScale().z));
    previewEffectAnimator = previewEffect.lock()->AcquireVFXAnimator();
    previewEffectAnimator.lock()->SetAutoActiveFalse();
    previewEffectAnimator.lock()->Init();
    previewEffectAnimator.lock()->Play();

	chargeEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Skill1");
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
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

    float localForeswingDuration;
    if (pod.foreswingDuration > 0)
    {
        foreswingSpeed = spinStartTime / pod.foreswingDuration;
        localForeswingDuration = pod.foreswingDuration;
    }
    else
    {
        localForeswingDuration = spinStartTime;
    }
    animator.lock()->GetGI().SetPlaySpeed(foreswingSpeed);
    previewEffectAnimator.lock()->SetSpeed(foreswingSpeed);
    chargeEffectAnimator.lock()->SetSpeed(foreswingSpeed);

    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitSpinStart{ skill.lock()->owner, localForeswingDuration };

    while (waitSpinStart.Tick())
    {
        co_await std::suspend_always();
    }

    float localSkillDuration;
    if (pod.skillDuration > 0)
    {
        skillSpeed = spinAttackingTime / pod.skillDuration;
        localSkillDuration = pod.skillDuration;
    }
    else
    {
        localSkillDuration = spinAttackingTime;
    }
    animator.lock()->GetGI().SetPlaySpeed(skillSpeed);
    chargeEffectAnimator.lock()->SetSpeed(skillSpeed);

    wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ skill.lock()->owner, localForeswingDuration };
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
            //each->Paralyze(pod.paralyzingDuration);
            each->KnockBack(owner.lock()->GetTransform()->GetWorldPosition() + delta, pod.knockBackDuration);
        }
    }

    float localBackswingDuration;
    if (pod.backswingDuration > 0)
    {
        backswingSpeed = (wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Spin)->GetDuration() - spinStartTime - spinAttackingTime) / pod.backswingDuration;
        localBackswingDuration = pod.backswingDuration;
    }
    else
    {
        localBackswingDuration = wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Spin)->GetDuration() - localForeswingDuration - localSkillDuration;
    }
    animator.lock()->GetGI().SetPlaySpeed(backswingSpeed);

    wanderUtils::UnitCoroutine::ForSecondsFromUnit afterDamagedDelay{ skill.lock()->owner, localBackswingDuration };

    while (afterDamagedDelay.Tick())
    {
        co_await std::suspend_always();
    }

    co_return;
}
