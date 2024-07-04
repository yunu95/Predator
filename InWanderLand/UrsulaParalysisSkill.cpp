#include "InWanderLand.h"
#include "UrsulaParalysisSkill.h"
#include "VFXAnimator.h"
#include "BossSummonMobSkill.h"

#include <math.h>

const float paralysisForeswingTime = 2.1f;

POD_UrsulaParalysisSkill UrsulaParalysisSkill::pod = POD_UrsulaParalysisSkill();

coroutine::Coroutine UrsulaParalysisSkill::operator()()
{
    colliderEffectRatio = 3.0f * 0.5f;
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    animator = owner.lock()->GetAnimator();
    paralysisAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill2);

    owner.lock()->SetDefaultAnimation(UnitAnimType::None);
    owner.lock()->PlayAnimation(UnitAnimType::Skill2);

    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(dynamic_pointer_cast<UrsulaParalysisSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            tentacleAnimator->GetGI().SetPlaySpeed(1);
            waveAnimator->GetGI().SetPlaySpeed(1);
            waveVFXAnimator.lock()->SetSpeed(1);

            FBXPool::Instance().Return(tentacleObject);
            FBXPool::Instance().Return(waveObject);
            UnitAcquisitionSphereColliderPool::Instance().Return(damageCollider);
            UnitAcquisitionSphereColliderPool::Instance().Return(knockBackCollider);
        });

    while (!animator.lock()->IsDone())
    {
        co_await std::suspend_always{};
    }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
        owner.lock()->OrderAttackMove();
    co_return;
}

coroutine::Coroutine UrsulaParalysisSkill::SpawningFieldEffect(std::weak_ptr<UrsulaParalysisSkill> skill)
{
    colliderEffectRatio = 3.0f * 0.5f;
    auto persistance = skill.lock();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    float actualCollideRange = GetSkillRadius() * (1 / colliderEffectRatio);

    damageCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    damageCollider.lock()->SetRadius(GetSkillRadius());
    damageCollider.lock()->GetTransform()->SetWorldPosition(targetPos);
    knockBackCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    knockBackCollider.lock()->SetRadius(GetSkillRadius());
    knockBackCollider.lock()->GetTransform()->SetWorldPosition(targetPos);

    tentacleObject = FBXPool::Instance().Borrow("SVFX_Ursula_Skill2_Tentacle");
    waveObject = FBXPool::Instance().Borrow("SVFX_Ursula_Skill2_Wave");
    tentacleObject.lock()->GetTransform()->SetWorldScale({ actualCollideRange, actualCollideRange, actualCollideRange });
    waveObject.lock()->GetTransform()->SetWorldScale({ actualCollideRange, actualCollideRange, actualCollideRange });

    tentacleAnimator = tentacleObject.lock()->GetGameObject()->GetChildren()[0]->GetComponent<yunutyEngine::graphics::Animator>();
    waveAnimator = waveObject.lock()->GetGameObject()->GetChildren()[0]->GetComponent<yunutyEngine::graphics::Animator>();
    
    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    auto& animList = rsrcManager->GetAnimationList();

    yunuGI::IAnimation* tentacleAnimation;
    yunuGI::IAnimation* waveAnimation;

    for (auto each : animList)
    {
        if (each->GetName() == L"root|Ani_SVFX_Tentacle")
        {
            tentacleAnimation = each;
            tentacleAnimator->PushAnimation(tentacleAnimation);
        }
        if (each->GetName() == L"root|Ani_SVFX_Wave")
        {
            waveAnimation = each;
            waveAnimator->PushAnimation(waveAnimation);
        }
    }

    waveVFXAnimator = waveObject.lock()->AcquireVFXAnimator();
    waveVFXAnimator.lock()->SetAutoActiveFalse();
    waveVFXAnimator.lock()->Init();
    tentacleObject.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos);
    waveObject.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos);
    tentacleObject.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    waveObject.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));

    if (pod.foreswingDuration > 0)
    {
        foreswingSpeed = paralysisForeswingTime / pod.foreswingDuration;
    }
    
    tentacleAnimator->GetGI().SetPlaySpeed(foreswingSpeed);
    waveAnimator->GetGI().SetPlaySpeed(foreswingSpeed);
    waveVFXAnimator.lock()->SetSpeed(foreswingSpeed);

    tentacleAnimator->Play(tentacleAnimation);
    waveAnimator->Play(waveAnimation);
    waveVFXAnimator.lock()->Play();

    co_await std::suspend_always{};

    bool hit = false;
    while (!hit)
    {
        auto curFrame = tentacleAnimator->GetCurrentFrame();

        if (curFrame >= 30)
        {
            hit = true;
            if (pod.skillDuration > 0)
            {
                skillSpeed = (paralysisAnim->GetDuration() - paralysisForeswingTime) / pod.skillDuration;
            }

            tentacleAnimator->GetGI().SetPlaySpeed(skillSpeed);
            waveAnimator->GetGI().SetPlaySpeed(skillSpeed);
            waveVFXAnimator.lock()->SetSpeed(skillSpeed);
        }

        if (hit)
        {
            co_await std::suspend_always{};
            for (auto& each : damageCollider.lock()->GetEnemies())
            {
                each->Damaged(owner, GetSkillDamage());
            }
        }
        co_await std::suspend_always{};
    }

    bool knockBacked = false;
    while (!knockBacked)
    {
        auto curFrame = tentacleAnimator->GetCurrentFrame();

        if (curFrame >= 70)
        {
            knockBacked = true;
        }

        if (knockBacked)
        {
            co_await std::suspend_always{};
            for (auto& each : knockBackCollider.lock()->GetEnemies())
            {
                if (BossSummonMobSkill::GetRightFrameUnit().expired() || (each != BossSummonMobSkill::GetLeftFrameUnit().lock().get() && each != BossSummonMobSkill::GetRightFrameUnit().lock().get()))
                {
                    each->KnockBack(targetPos, pod.knockBackDuration);
	    			each->Paralyze(pod.skillParalysisTime);
                }
            }
        }
        co_await std::suspend_always{};
    }

    while (!waveVFXAnimator.lock()->IsDone())
    {
        co_await std::suspend_always{};
    }

    co_return;
}

void UrsulaParalysisSkill::OnInterruption()
{
    animator.lock()->GetGI().SetPlaySpeed(1);
}

float UrsulaParalysisSkill::GetSkillRadius()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::URSULA_W_RADIUS) ? pod.skillRadiusUpgraded : pod.skillRadius;
}

float UrsulaParalysisSkill::GetSkillDamage()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::URSULA_W_DAMAGE) ? pod.skillDamageUpgraded : pod.skillDamage;
}
