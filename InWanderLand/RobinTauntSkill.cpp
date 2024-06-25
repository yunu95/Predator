#include "InWanderLand.h"
#include "RobinTauntSkill.h"
#include "VFXAnimator.h"

POD_RobinTauntSkill RobinTauntSkill::pod = POD_RobinTauntSkill();

const float damageTimingFrame = 24.0f;

coroutine::Coroutine RobinTauntSkill::operator()()
{
    colliderEffectRatio = 6.0f * 0.5f;
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    auto animator = owner.lock()->GetAnimator();
    auto tauntAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Taunt);
    tauntAnim->SetLoop(false);

    owner.lock()->SetDesiredRotation((owner.lock()->GetTransform()->GetWorldRotation().Forward()));

    owner.lock()->PlayAnimation(UnitAnimType::Taunt, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);

    while (animator.lock()->GetCurrentAnimation() != wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Taunt))
    {
        co_await std::suspend_always{};
    }

    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(std::dynamic_pointer_cast<RobinTauntSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            tauntCollider.lock()->SetRadius(0.5);
            UnitAcquisitionSphereColliderPool::Instance().Return(tauntCollider);
            FBXPool::Instance().Return(tauntEffect);
        });

    co_yield coroutine::WaitForSeconds(tauntAnim->GetDuration());

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    co_return;
}

void RobinTauntSkill::OnInterruption()
{
    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
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
    tauntEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    auto chargeEffectAnimator = tauntEffect.lock()->AcquireVFXAnimator();
    chargeEffectAnimator.lock()->SetAutoActiveFalse();
    chargeEffectAnimator.lock()->Init();
    chargeEffectAnimator.lock()->Play();

    auto animator = owner.lock()->GetAnimator();
    auto tauntAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Taunt);

    tauntCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    tauntCollider.lock()->SetRadius(GetSkillRadius());
    tauntCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    coroutine::ForSeconds forSeconds{ tauntAnim->GetDuration() };
    std::unordered_set<Unit*> tauntList;

    co_await std::suspend_always{};

    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};

        if (animator.lock()->GetCurrentFrame() >= damageTimingFrame)
        {
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
    }
    co_await std::suspend_always{};

    co_return;
}
