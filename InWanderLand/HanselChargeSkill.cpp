#include "InWanderLand.h"
#include "HanselChargeSkill.h"
#include "VFXAnimator.h"

POD_HanselChargeSkill HanselChargeSkill::pod = POD_HanselChargeSkill();

#define _USE_MATH_DEFINES
#include <math.h>

const float	JumpAndLandDuration = 0.88f;
const float damageDelay = 0.92f;
const float jumpForeswingTime = 0.2f;

coroutine::Coroutine HanselChargeSkill::operator()()
{
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill1);

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;
    animator = owner.lock()->GetAnimator();

    owner.lock()->SetDefaultAnimation(UnitAnimType::None);
    owner.lock()->PlayAnimation(UnitAnimType::Skill1);

    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(dynamic_pointer_cast<HanselChargeSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            animator.lock()->GetGI().SetPlaySpeed(1);
            stompEffect1Animator.lock()->SetSpeed(1);
            //stompEffect2Animator.lock()->SetSpeed(1);
            UnitAcquisitionSphereColliderPool::Instance().Return(stompCollider);
            FBXPool::Instance().Return(stompEffect1);
            //FBXPool::Instance().Return(stompEffect2);
        });
    float localForeswingDuration;
    if (pod.foreswingDuration > 0)
    {
        localForeswingDuration = pod.foreswingDuration;
    }
    else
    {
        localForeswingDuration = jumpForeswingTime;
    }
    float localSkillDuration;
    if (pod.skillDuration > 0)
    {
        localSkillDuration = pod.skillDuration;
    }
    else
    {
        localSkillDuration = JumpAndLandDuration + damageDelay;
    }
    float localBackswingDuration;
    if (pod.backswingDuration > 0)
    {
        localBackswingDuration = pod.backswingDuration;
    }
    else
    {
        localBackswingDuration = anim->GetDuration() - jumpForeswingTime - JumpAndLandDuration - damageDelay;
    }

    co_yield coroutine::WaitForSeconds(localForeswingDuration + localSkillDuration + localBackswingDuration);

    if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
        owner.lock()->OrderAttackMove();

    co_return;
}

void HanselChargeSkill::OnInterruption()
{
    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
}

float HanselChargeSkill::GetMaxRange()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::HANSEL_Q_RANGE) ? pod.maxRangeUpgraded : pod.maxRange;
}

float HanselChargeSkill::GetDamage()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::HANSEL_Q_DAMAGE) ? pod.damageUpgraded : pod.damage;
}

coroutine::Coroutine HanselChargeSkill::SpawningFieldEffect(std::weak_ptr<HanselChargeSkill> skill)
{
    auto persistance = skill.lock();
    colliderEffectRatio = 10.0f * 0.5f;
    float actualCollideRange = pod.skillRadius * 1 / (colliderEffectRatio);

    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    float localSkillDuration;
    float localJumpDuration;
    float localDamageDuration;
    if (pod.skillDuration > 0)
    {
        jumpSpeed = JumpAndLandDuration / (JumpAndLandDuration * pod.skillDuration / (JumpAndLandDuration + damageDelay));
        skillSpeed = damageDelay / (damageDelay * pod.skillDuration / (JumpAndLandDuration + damageDelay));
        localSkillDuration = pod.skillDuration;
        localJumpDuration = (JumpAndLandDuration * pod.skillDuration / (JumpAndLandDuration + damageDelay));
        localDamageDuration = (damageDelay * pod.skillDuration / (JumpAndLandDuration + damageDelay));
    }
    else
    {
        localSkillDuration = JumpAndLandDuration + damageDelay;
        localJumpDuration = JumpAndLandDuration;
        localDamageDuration = damageDelay;
    }

    stompCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    stompCollider.lock()->SetRadius(pod.skillRadius);
    stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    /// 이펙트도 생성
    stompEffect1 = FBXPool::Instance().Borrow("VFX_Hansel_Skill1_1");
    stompEffect1.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos);
    stompEffect1.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    stompEffect1.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));
    //stompEffect1.lock()->GetGameObject()->SetSelfActive(false);

    //stompEffect2 = FBXPool::Instance().Borrow("VFX_Hansel_Skill1_2");
    //stompEffect2.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos);
    //stompEffect2.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    //stompEffect2.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));
    //stompEffect2.lock()->GetGameObject()->SetSelfActive(false);

    stompEffect1Animator = stompEffect1.lock()->AcquireVFXAnimator();
    stompEffect1Animator.lock()->SetAutoActiveFalse();
    stompEffect1Animator.lock()->Init();

    stompEffect1.lock()->GetGameObject()->SetSelfActive(true);
    stompEffect1Animator.lock()->SetSpeed(skillSpeed);
    stompEffect1Animator.lock()->Play();

    //stompEffect2Animator = stompEffect2.lock()->AcquireVFXAnimator();
    //stompEffect2Animator.lock()->SetAutoActiveFalse();
    //stompEffect2Animator.lock()->Init();

    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill1);

    float localForeswingDuration;
    if (pod.foreswingDuration > 0)
    {
        foreswingSpeed = jumpForeswingTime / pod.foreswingDuration;
        localForeswingDuration = pod.foreswingDuration;
    }
    else
    {
        localForeswingDuration = jumpForeswingTime;
    }
    animator.lock()->GetGI().SetPlaySpeed(foreswingSpeed);

    co_yield coroutine::WaitForSeconds(localForeswingDuration);

    animator.lock()->GetGI().SetPlaySpeed(jumpSpeed);

    float rushSpeed = static_cast<float>(deltaPos.Magnitude()) / localJumpDuration;
    coroutine::ForSeconds forSeconds{ localJumpDuration };

    // y = vy0 * t - 0.5 * a * t^2
    // y가 0일 때, t는 Duration이고, t = Duration / 2 일 때, y는 jumpDistance.
    // 연립방정식 풀면,
    // a = (8 * jumpDistance) / Duration^2
    // vy0 = 4 * jumpDistance / Duration

    float vy0 = 4 * pod.maxJumpHeight / localJumpDuration;
    float acc = (8 * pod.maxJumpHeight) / (localJumpDuration * localJumpDuration);
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
    animator.lock()->GetGI().SetPlaySpeed(skillSpeed);
    //stompEffect2Animator.lock()->SetSpeed(skillSpeed);

    stompCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    co_await std::suspend_always{};

    //stompEffect2.lock()->GetGameObject()->SetSelfActive(true);
    // stompEffect2Animator.lock()->Play();

    for (auto each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, GetDamage());
        SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::HitSkill_Hansel_Q), each->GetTransform()->GetWorldPosition());
    }
    RTSCam::Instance().ApplyShake(pod.impactCamShakeDistance1, pod.impactCamShakeFrequency1, pod.impactCamShakeDecreaseFactor1, endPos);

    while (animator.lock()->GetCurrentFrame() < 60)
    {
        co_await std::suspend_always{};
    }

    for (auto each : stompCollider.lock()->GetEnemies())
    {
        each->Damaged(owner, GetDamage());
        SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::HitSkill_Hansel_Q), each->GetTransform()->GetWorldPosition());
        Vector3d delta = pod.impactKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();

        if (BossSummonMobSkill::GetRightFrameUnit().expired() || (each != BossSummonMobSkill::GetLeftFrameUnit().lock().get() && each != BossSummonMobSkill::GetRightFrameUnit().lock().get()))
        {
            each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, pod.impactKnockbackDuration);
        }
    }
    RTSCam::Instance().ApplyShake(pod.impactCamShakeDistance2, pod.impactCamShakeFrequency2, pod.impactCamShakeDecreaseFactor2, endPos);

    float localBackswingDuration;
    if (pod.backswingDuration > 0)
    {
        backswingSpeed = (anim->GetDuration() - jumpForeswingTime - JumpAndLandDuration - damageDelay) / pod.backswingDuration;
        localBackswingDuration = pod.backswingDuration;
    }
    else
    {
        localBackswingDuration = anim->GetDuration() - jumpForeswingTime - JumpAndLandDuration - damageDelay;
    }

    animator.lock()->GetGI().SetPlaySpeed(backswingSpeed);
    stompEffect1Animator.lock()->SetSpeed(backswingSpeed);
    //stompEffect2Animator.lock()->SetSpeed(backswingSpeed);

    wanderUtils::UnitCoroutine::ForSecondsFromUnit backForSeconds{ owner, localBackswingDuration };

    while (backForSeconds.Tick())
    {
        co_await std::suspend_always{};
    }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(currentPos);

    co_return;
}
