#include "InWanderLand.h"
#include "RobinTauntSkill.h"
#include "VFXAnimator.h"

POD_RobinTauntSkill RobinTauntSkill::pod = POD_RobinTauntSkill();

const float tauntForeswingTime = 0.24f;

coroutine::Coroutine RobinTauntSkill::operator()()
{
    colliderEffectRatio = 6.0f * 0.5f;
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto enableNavObstacle = owner.lock()->referenceEnableNavObstacle.Acquire();
    auto blockRot = owner.lock()->referenceBlockRotation.Acquire();

    animator = owner.lock()->GetAnimator();
    tauntAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Taunt);
    tauntAnim->SetLoop(false);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    if (pod.foreswingDuration > 0)
    {
        foreswingSpeed = tauntForeswingTime / pod.foreswingDuration;
    }

    owner.lock()->SetDefaultAnimation(UnitAnimType::None);
    animator.lock()->GetGI().SetPlaySpeed(foreswingSpeed);
    owner.lock()->PlayAnimation(UnitAnimType::Taunt);

    auto beforeSkillZoomFactor = PlayerController::Instance().GetZoomFactor();

    PlayerController::Instance().SetZoomFactor(beforeSkillZoomFactor * pod.zoomFactor);

    co_yield coroutine::WaitForSeconds(tauntForeswingTime / foreswingSpeed);

    PlayerController::Instance().SetZoomFactor(beforeSkillZoomFactor);

    animator.lock()->Pause();

    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(std::dynamic_pointer_cast<RobinTauntSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            tauntCollider.lock()->SetRadius(0.5);
            UnitAcquisitionSphereColliderPool::Instance().Return(tauntCollider);
            FBXPool::Instance().Return(tauntEffect);
            animator.lock()->GetGI().SetPlaySpeed(1);
            if (!chargeEffectAnimator.expired())
            {
                chargeEffectAnimator.lock()->SetSpeed(1);
            }
        });

    float localForeswingDuration;
    float localSkillDuration;

    if (pod.foreswingDuration > 0)
    {
        localForeswingDuration = pod.foreswingDuration;
    }
    else
    {
        localForeswingDuration = tauntForeswingTime;
    }

    if (pod.skillDuration > 0)
    {
        localSkillDuration = pod.skillDuration;
    }
    else
    {
        localSkillDuration = tauntAnim->GetDuration() - localForeswingDuration;
    }

    co_yield coroutine::WaitForSeconds(localForeswingDuration + localSkillDuration);

    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
        owner.lock()->OrderAttackMove();
    co_return;
}

void RobinTauntSkill::OnInterruption()
{
    animator.lock()->GetGI().SetPlaySpeed(1);

    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
    if (!chargeEffectAnimator.expired())
    {
        chargeEffectAnimator.lock()->SetSpeed(1);
    }
}

float RobinTauntSkill::GetSkillRadius()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_W_RADIUS) ? pod.skillRadiusUpgraded : pod.skillRadius;
}
float RobinTauntSkill::GetSkillDamage()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_W_DAMAGE) ? pod.skillDamageUpgraded : pod.skillDamage;
}
coroutine::Coroutine RobinTauntSkill::SpawningSkillffect(std::weak_ptr<RobinTauntSkill> skill)
{
    auto animator = owner.lock()->GetAnimator();

    colliderEffectRatio = 6.0f * 0.5f;

    auto persistance = skill.lock();

    float actualCollideRange = GetSkillRadius() * (1 / colliderEffectRatio);
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    tauntEffect = FBXPool::Instance().Borrow("VFX_Robin_Skill2");
    tauntEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    tauntEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));
    tauntEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(RTSCam::Instance().GetTransform()->GetWorldRotation());
    chargeEffectAnimator = tauntEffect.lock()->AcquireVFXAnimator();
    chargeEffectAnimator.lock()->SetAutoActiveFalse();
    chargeEffectAnimator.lock()->Init();
    if (pod.skillDuration > 0)
    {
        skillSpeed = (tauntAnim->GetDuration() - tauntForeswingTime) / pod.skillDuration;
    }
    chargeEffectAnimator.lock()->SetSpeed(skillSpeed);
    chargeEffectAnimator.lock()->Play();
    animator.lock()->GetGI().SetPlaySpeed(skillSpeed);
    animator.lock()->Resume();

    tauntCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    tauntCollider.lock()->SetRadius(GetSkillRadius());
    tauntCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    coroutine::ForSeconds forSeconds{ (tauntAnim->GetDuration() - tauntForeswingTime) / skillSpeed };
    std::unordered_set<Unit*> tauntList;

    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};

        for (auto& each : tauntCollider.lock()->GetEnemies())
        {
            if (!tauntList.contains(each))
            {
                tauntList.insert(each);
                /// 도발
                /// 도발 대상은 skillTauntTime 동안 Robin 공격하게 되어야 함
                each->ApplyBuff(UnitBuffTaunted{ owner });
                each->OrderAttack(owner);
                each->Damaged(owner, GetSkillDamage());
            }
        }
    }
    co_await std::suspend_always{};

    co_return;
}
