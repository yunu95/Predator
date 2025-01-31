#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"
#include "VFXAnimator.h"

#include <math.h>

Vector3d UrsulaBlindSkill::skillStart = Vector3d();
Vector3d UrsulaBlindSkill::skillDestination = Vector3d();

POD_UrsulaBlindSkill UrsulaBlindSkill::pod = POD_UrsulaBlindSkill();

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
    colliderEffectRatio = 3.0f * 0.5f;
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    auto enableNavObstacle = owner.lock()->referenceEnableNavObstacle.Acquire();
    animator = owner.lock()->GetAnimator();
    skill1Anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill1);

    if (pod.skillDuration > 0)
    {
        skillSpeed = skill1Anim->GetDuration() / pod.skillDuration;
    }

    owner.lock()->SetDefaultAnimation(UnitAnimType::None);
    animator.lock()->GetGI().SetPlaySpeed(skillSpeed);
    owner.lock()->PlayAnimation(UnitAnimType::Skill1);

    UpdatePosition(owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition(), targetPos);

    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(dynamic_pointer_cast<UrsulaBlindSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            animator.lock()->GetGI().SetPlaySpeed(1);

            circle_Top.lock()->SetRadius(0.5);
            circle_Left.lock()->SetRadius(0.5);
            circle_Right.lock()->SetRadius(0.5);
            UnitAcquisitionSphereColliderPool::Instance().Return(circle_Top);
            UnitAcquisitionSphereColliderPool::Instance().Return(circle_Left);
            UnitAcquisitionSphereColliderPool::Instance().Return(circle_Right);
            FBXPool::Instance().Return(onUrsulaPosEffect);
            FBXPool::Instance().Return(onTargetPosEffect1);
            FBXPool::Instance().Return(onTargetPosEffect2);
            FBXPool::Instance().Return(onTargetPosEffect3);
        });

    float localSkillDuration = 0;

    if (pod.skillDuration > 0)
    {
        localSkillDuration = pod.skillDuration;
    }
    else
    {
        localSkillDuration = skill1Anim->GetDuration();
    }

    co_yield coroutine::WaitForSeconds(localSkillDuration);

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
        owner.lock()->OrderAttackMove();

    co_return;
}

void UrsulaBlindSkill::OnInterruption()
{
    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
    animator.lock()->GetGI().SetPlaySpeed(1);
}

coroutine::Coroutine UrsulaBlindSkill::SpawningFieldEffect(std::weak_ptr<UrsulaBlindSkill> skill)
{
    colliderEffectRatio = 3.0f * 0.5f;

    auto persistance = skill.lock();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    float actualCollideRange = pod.skillRadius * (1 / colliderEffectRatio);

    onUrsulaPosEffect = FBXPool::Instance().Borrow("VFX_Ursula_Skill1_1");
    onTargetPosEffect1 = FBXPool::Instance().Borrow("VFX_Ursula_Skill1_2");
    onTargetPosEffect2 = FBXPool::Instance().Borrow("VFX_Ursula_Skill1_2");
    onTargetPosEffect3 = FBXPool::Instance().Borrow("VFX_Ursula_Skill1_2");

    UpdatePosition(owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition(), targetPos);

    onUrsulaPosEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
    onTargetPosEffect1.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Top(skillDestination));
    onTargetPosEffect2.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Left(skillDestination));
    onTargetPosEffect3.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Right(skillDestination));

    onUrsulaPosEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    onTargetPosEffect1.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    onTargetPosEffect2.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
    onTargetPosEffect3.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));

    onUrsulaPosEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));
    onTargetPosEffect1.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));
    onTargetPosEffect2.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));
    onTargetPosEffect3.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));

    auto onUrsulaPosAnimator = onUrsulaPosEffect.lock()->AcquireVFXAnimator();
    onUrsulaPosAnimator.lock()->SetAutoActiveFalse();
    onUrsulaPosAnimator.lock()->Init();
    onUrsulaPosAnimator.lock()->SetSpeed(skillSpeed);
    onUrsulaPosAnimator.lock()->Play();

    auto onTargetPosAnimator1 = onTargetPosEffect1.lock()->AcquireVFXAnimator();
    onTargetPosAnimator1.lock()->SetAutoActiveFalse();
    onTargetPosAnimator1.lock()->Init();
    onTargetPosAnimator1.lock()->SetSpeed(skillSpeed);
    onTargetPosAnimator1.lock()->Play();

    auto onTargetPosAnimator2 = onTargetPosEffect2.lock()->AcquireVFXAnimator();
    onTargetPosAnimator2.lock()->SetAutoActiveFalse();
    onTargetPosAnimator2.lock()->Init();
    onTargetPosAnimator2.lock()->SetSpeed(skillSpeed);
    onTargetPosAnimator2.lock()->Play();

    auto onTargetPosAnimator3 = onTargetPosEffect3.lock()->AcquireVFXAnimator();
    onTargetPosAnimator3.lock()->SetAutoActiveFalse();
    onTargetPosAnimator3.lock()->Init();
    onTargetPosAnimator3.lock()->SetSpeed(skillSpeed);
    onTargetPosAnimator3.lock()->Play();

    circle_Top = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    circle_Left = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    circle_Right = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());

    circle_Top.lock()->SetRadius(pod.skillRadius);
    circle_Left.lock()->SetRadius(pod.skillRadius);
    circle_Right.lock()->SetRadius(pod.skillRadius);

    circle_Top.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Top(skillDestination));
    circle_Left.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Left(skillDestination));
    circle_Right.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Right(skillDestination));

    co_await std::suspend_always{};

    int hitCount = 0;
    while (!onTargetPosAnimator1.lock()->IsDone())
    {
        auto curFrame = animator.lock()->GetCurrentFrame();
        bool hit = false;
        if (hitCount == 0 && (curFrame >= 22 && curFrame < 40))
        {
            hit = true;
            hitCount++;
        }
        else if (hitCount == 1 && (curFrame >= 40 && curFrame < 58))
        {
            hit = true;
            hitCount++;
        }
        else if (hitCount == 2 && curFrame >= 58)
        {
            hit = true;
            hitCount++;
        }

        if (hit)
        {
            co_await std::suspend_always{};

            for (auto& each : circle_Top.lock()->GetEnemies())
            {
                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
                each->ApplyBuff(UnitBuffBlinded{});
                each->Damaged(owner, GetSkillDamage());
				SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::HitSkill_Ursula_Q), each->GetTransform()->GetWorldPosition());
            }

            for (auto& each : circle_Left.lock()->GetEnemies())
            {
                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
                each->ApplyBuff(UnitBuffBlinded{});
                each->Damaged(owner, GetSkillDamage());
				SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::HitSkill_Ursula_Q), each->GetTransform()->GetWorldPosition());
            }

            for (auto& each : circle_Right.lock()->GetEnemies())
            {
                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
                each->ApplyBuff(UnitBuffBlinded{});
                each->Damaged(owner, GetSkillDamage());
				SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::HitSkill_Ursula_Q), each->GetTransform()->GetWorldPosition());
            }
        }

        /// 우선은 여러 영역 겹칠 경우, 중복하여 대미지 계산함
        co_await std::suspend_always{};
    }

    while (!onTargetPosAnimator1.lock()->IsDone())
    {
        co_await std::suspend_always{};
    }

    co_return;
}

void UrsulaBlindSkill::UpdatePosition(const Vector3d& start, const Vector3d& dest)
{
    skillStart = start;
    skillDestination = dest;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Top(const Vector3d& dest)
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto skillDir = (dest - skillStart).Normalized();
    return dest + std::sqrt(3) / 3 * skillDir * length;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Left(const Vector3d& dest)
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto skillDir = (dest - skillStart).Normalized();
    auto left = Vector3d::Cross(skillDir, Vector3d::up).Normalized();
    return dest - std::sqrt(3) / 6 * skillDir * length + left * length / 2;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Right(const Vector3d& dest)
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto skillDir = (dest - skillStart).Normalized();
    auto right = Vector3d::Cross(-skillDir, Vector3d::up).Normalized();
    return dest - std::sqrt(3) / 6 * skillDir * length + right * length / 2;
}

float UrsulaBlindSkill::GetSkillRange()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::URSULA_Q_RANGE) ? pod.skillRangeUpgraded : pod.skillRange;
}

float UrsulaBlindSkill::GetSkillDamage()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::URSULA_Q_DAMAGE) ? pod.skillDamageUpgraded : pod.skillDamage;
}


