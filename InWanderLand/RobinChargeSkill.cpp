#include "InWanderLand.h"
#include "RobinChargeSkill.h"
#include "VFXAnimator.h"
#include "BossSummonMobSkill.h"

POD_RobinChargeSkill RobinChargeSkill::pod = POD_RobinChargeSkill();

coroutine::Coroutine RobinChargeSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    if (deltaPos.Magnitude() > GetMaxDistance())
    {
        deltaPos = direction * GetMaxDistance();
    }
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    auto animator = owner.lock()->GetAnimator();
    auto rushAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Rush);
    auto slamAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Slam);

    owner.lock()->PlayAnimation(UnitAnimType::Rush, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);

    co_yield coroutine::WaitForSeconds(rushAnim->GetDuration());

    animator.lock()->Pause();

    co_await std::suspend_always{};

    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / pod.rushSpeed };
    knockbackCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    knockbackCollider.lock()->SetRadius(pod.rushKnockbackRadius);
    while (forSeconds.Tick())
    {
        currentPos += direction * pod.rushSpeed * Time::GetDeltaTime();
        knockbackCollider.lock()->GetTransform()->SetWorldPosition(currentPos);
        owner.lock()->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
        for (auto& each : knockbackCollider.lock()->GetEnemies())
        {
            Vector3d delta = pod.rushKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
            each->Damaged(owner, GetDamageRush());
            if (each != BossSummonMobSkill::GetLeftFrameUnit().lock().get() && each != BossSummonMobSkill::GetRightFrameUnit().lock().get())
            {
                each->KnockBackRelativeVector(delta, pod.rushKnockbackDuration);
            }
        }
    }

    owner.lock()->PlayAnimation(UnitAnimType::Slam);
    animator.lock()->Resume();
    knockbackCollider.lock()->SetRadius(pod.impactKnockbackRadius);
    auto effectCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(dynamic_pointer_cast<RobinChargeSkill>(selfWeakPtr.lock()), startPos));
    effectCoroutine.lock()->PushDestroyCallBack([this]()
        {
            FBXPool::Instance().Return(chargeEffect);
            UnitAcquisitionSphereColliderPool::Instance().Return(knockbackCollider);
        });

    co_await std::suspend_always{};
    for (auto& each : knockbackCollider.lock()->GetEnemies())
    {
        Vector3d delta = pod.impactKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
        //each->Paralyze(pod.impactStunDuration);
        each->Damaged(owner, GetDamageImpact());
        if (BossSummonMobSkill::GetRightFrameUnit().expired() || (each != BossSummonMobSkill::GetLeftFrameUnit().lock().get() && each != BossSummonMobSkill::GetRightFrameUnit().lock().get()))
        {
            each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, pod.impactKnockbackDuration);
        }
    }
    RTSCam::Instance().ApplyShake(pod.impactCamShakeDistance, pod.impactCamShakeFrequency, pod.impactCamShakeDecreaseFactor);

    co_yield coroutine::WaitForSeconds(slamAnim->GetDuration());

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(currentPos);
    owner.lock()->PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
    co_yield coroutine::WaitForSeconds(0.2);
    co_return;
}

void RobinChargeSkill::OnInterruption()
{

}

float RobinChargeSkill::GetMaxDistance()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_Q_RANGE) ? pod.maxDistanceUpgraded : pod.maxDistance;
}

float RobinChargeSkill::GetDamageRush()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_Q_DAMAGE) ? pod.damageRushUpgraded : pod.damageRush;
}

float RobinChargeSkill::GetDamageImpact()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_Q_DAMAGE) ? pod.damageImpactUpgraded : pod.damageImpact;
}

coroutine::Coroutine RobinChargeSkill::SpawningSkillffect(std::weak_ptr<RobinChargeSkill> skill, Vector3d skillStartPos)
{
    auto persistance = skill.lock();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - skillStartPos;
    Vector3d direction = deltaPos.Normalized();

    chargeEffect = FBXPool::Instance().Borrow("VFX_Robin_Skill1");

    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.effectScale, pod.effectScale, pod.effectScale));

    auto chargeEffectAnimator = chargeEffect.lock()->AcquireVFXAnimator();
    chargeEffectAnimator.lock()->SetAutoActiveFalse();
    chargeEffectAnimator.lock()->Init();
    chargeEffectAnimator.lock()->Play();

    co_await std::suspend_always{};

    while (!chargeEffectAnimator.lock()->IsDone())
    {
        co_await std::suspend_always{};
    }

    co_return;
}