#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"
#include "VFXAnimator.h"

#include <math.h>

Vector3d UrsulaBlindSkill::skillStart = Vector3d();
Vector3d UrsulaBlindSkill::skillDestination = Vector3d();

POD_UrsulaBlindSkill UrsulaBlindSkill::pod = POD_UrsulaBlindSkill();

float UrsulaBlindSkill::colliderEffectRatio = 3.0f * 0.5f;

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    auto enableNavObstacle = owner.lock()->referenceEnableNavObstacle.Acquire();
    owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
    auto animator = owner.lock()->GetAnimator();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill1);

    UpdatePosition(owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition(), targetPos);

    effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(dynamic_pointer_cast<UrsulaBlindSkill>(selfWeakPtr.lock())));
    effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
        {
            circle_Top.lock()->SetRadius(0.5);
            circle_Left.lock()->SetRadius(0.5);
            circle_Right.lock()->SetRadius(0.5);
            UnitAcquisitionSphereColliderPool::Instance().Return(circle_Top);
            UnitAcquisitionSphereColliderPool::Instance().Return(circle_Left);
            UnitAcquisitionSphereColliderPool::Instance().Return(circle_Right);
            FBXPool::SingleInstance().Return(onUrsulaPosEffect);
            FBXPool::SingleInstance().Return(onTargetPosEffect1);
            FBXPool::SingleInstance().Return(onTargetPosEffect2);
            FBXPool::SingleInstance().Return(onTargetPosEffect3);
        });

    co_yield coroutine::WaitForSeconds(anim->GetDuration());

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    co_return;
}

void UrsulaBlindSkill::OnInterruption()
{
    if (!effectColliderCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectColliderCoroutine);
    }
}

coroutine::Coroutine UrsulaBlindSkill::SpawningFieldEffect(std::weak_ptr<UrsulaBlindSkill> skill)
{
    auto persistance = skill.lock();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    float actualCollideRange = pod.skillRadius * (1 / colliderEffectRatio);

    onUrsulaPosEffect = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_1");
    onTargetPosEffect1 = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_2");
    onTargetPosEffect2 = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_2");
    onTargetPosEffect3 = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_2");

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

    auto onTargetPosAnimator1 = onTargetPosEffect1.lock()->AcquireVFXAnimator();
    onTargetPosAnimator1.lock()->SetAutoActiveFalse();
    onTargetPosAnimator1.lock()->Init();

    auto onTargetPosAnimator2 = onTargetPosEffect2.lock()->AcquireVFXAnimator();
    onTargetPosAnimator2.lock()->SetAutoActiveFalse();
    onTargetPosAnimator2.lock()->Init();

    auto onTargetPosAnimator3 = onTargetPosEffect3.lock()->AcquireVFXAnimator();
    onTargetPosAnimator3.lock()->SetAutoActiveFalse();
    onTargetPosAnimator3.lock()->Init();

    auto animator = owner.lock()->GetAnimator();

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
                each->Damaged(owner, pod.skillDamage);
                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
            }

            for (auto& each : circle_Left.lock()->GetEnemies())
            {
                each->Damaged(owner, pod.skillDamage);
                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
            }

            for (auto& each : circle_Right.lock()->GetEnemies())
            {
                each->Damaged(owner, pod.skillDamage);
                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
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


