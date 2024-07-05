#include "InWanderLand.h"
#include "HanselProjectileSkill.h"
#include "VFXAnimator.h"

const float throwingPieTimingFrame = 43.0f;
const float throwingPieTiming = 1.13f;
POD_HanselProjectileSkill HanselProjectileSkill::pod = POD_HanselProjectileSkill();

coroutine::Coroutine HanselProjectileSkill::ThrowingPie(std::weak_ptr<HanselProjectileSkill> skill)
{
    colliderEffectRatio = 1.0f * 0.5f;
    float actualCollideRange = pod.skillRadius * (1 / colliderEffectRatio);

    auto persistance = skill.lock();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / pod.projectileSpeed };
    pieCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
    pieObject = FBXPool::Instance().Borrow("SM_Pie");
    pieObject.lock()->GetGameObject()->SetSelfActive(false);

    std::unordered_set<Unit*> onceCollidedUnits;
    co_await std::suspend_always{};

    pieCollider.lock()->SetRadius(pod.skillRadius);
    pieObject.lock()->GetTransform()->SetWorldScale({ actualCollideRange, actualCollideRange, actualCollideRange });
    pieCollider.lock()->GetTransform()->SetWorldRotation(direction);
    auto pieStartRotation = Quaternion::MakeWithForwardUp(direction.up * -1, direction);

    pieObject.lock()->GetTransform()->SetWorldRotation(pieStartRotation);
    //pieObject->GetTransform()->SetWorldRotation(pieObject->GetTransform()->GetWorldRotation().Up() * -1);
    float rotatePerFrame = 0.0f;

    while (forSeconds.Tick())
    {
        currentPos += direction * pod.projectileSpeed * Time::GetDeltaTime();
        pieCollider.lock()->GetTransform()->SetWorldPosition(currentPos + direction * pod.pieOffsetX + Vector3d(0, pod.pieOffsetY, 0));
        pieObject.lock()->GetTransform()->SetWorldPosition(currentPos + direction * pod.pieOffsetX + Vector3d(0, pod.pieOffsetY, 0));

        rotatePerFrame += pod.pieRotateSpeed * Time::GetDeltaTime();

        Vector3d directionPerFrame = (endPos - currentPos).Normalized();
        //pieObject->GetTransform()->SetWorldRotation(pieObject->GetTransform()->GetWorldRotation() + Vector3d(rotatePerFrame, 0, 0));
        auto euler = pieObject.lock()->GetTransform()->GetLocalRotation().Euler();
        euler.x = rotatePerFrame;
        pieObject.lock()->GetTransform()->SetWorldRotation(Quaternion{ euler });
        co_await std::suspend_always{};
        pieObject.lock()->GetGameObject()->SetSelfActive(true);
        for (auto& each : pieCollider.lock()->GetEnemies())
        {
            if (onceCollidedUnits.find(each) == onceCollidedUnits.end())
            {
                /// 충돌한 적군에게는 실명(디버프)을 부여합니다.
                onceCollidedUnits.insert(each);
                //each->StartCoroutine(SpawningSkillffect(each->GetWeakPtr<Unit>()));
                each->ApplyBuff(UnitBuffHanselDebuff{});
            }
        }

        for (auto& each : pieCollider.lock()->GetFriends())
        {
            if (onceCollidedUnits.find(each) == onceCollidedUnits.end())
            {
                /// 충돌한 아군에게는 음향버프 및 체력회복을 부여합니다.
                onceCollidedUnits.insert(each);
                each->ApplyBuff(UnitBuffHanselBuff{});
                each->Heal(pod.healPoint);

            }
        }
    }

    co_return;
}

float HanselProjectileSkill::GetCastRange()
{
    return GetMaxRange();
}

coroutine::Coroutine HanselProjectileSkill::operator()()
{
    colliderEffectRatio = 1.0f * 0.5f;
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    animator = owner.lock()->GetAnimator();
    throwAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Throw);

    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);

    float localPieTiming;
    if (pod.foreswingDuration > 0)
    {
        foreswingSpeed = throwingPieTiming / pod.foreswingDuration;
        localPieTiming = pod.foreswingDuration;
    }
    else
    {
        localPieTiming = throwingPieTiming;
    }

    owner.lock()->SetDefaultAnimation(UnitAnimType::None);
    animator.lock()->GetGI().SetPlaySpeed(foreswingSpeed);
    owner.lock()->PlayAnimation(UnitAnimType::Throw);

    while (animator.lock()->GetCurrentAnimation() != wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Throw)
        || throwingPieTimingFrame >= animator.lock()->GetCurrentFrame())
    {
        co_await std::suspend_always{};
    }

    auto throwingCoroutine = owner.lock()->StartCoroutine(ThrowingPie(std::dynamic_pointer_cast<HanselProjectileSkill>(selfWeakPtr.lock())));
    throwingCoroutine.lock()->PushDestroyCallBack([this]()
        {
            UnitAcquisitionSphereColliderPool::Instance().Return(pieCollider);
            FBXPool::Instance().Return(pieObject);
        });

    if (pod.backswingDuration > 0)
    {
        backswingSpeed = (throwAnim->GetDuration() - localPieTiming) / pod.backswingDuration;
    }
    animator.lock()->GetGI().SetPlaySpeed(backswingSpeed);

    while (!animator.lock()->IsDone())
    {
        co_await std::suspend_always{};
    }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    OnInterruption();
    if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
        owner.lock()->OrderAttackMove();
    co_return;
}

void HanselProjectileSkill::OnInterruption()
{
    animator.lock()->GetGI().SetPlaySpeed(1);
}

float HanselProjectileSkill::GetMaxRange()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::HANSEL_W_RANGE) ? pod.maxRangeUpgraded : pod.maxRange;
}

int HanselProjectileSkill::GetHitCount()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::HANSEL_W_MORE_HITS) ? pod.hitCountUpgraded : pod.hitCount;
}
