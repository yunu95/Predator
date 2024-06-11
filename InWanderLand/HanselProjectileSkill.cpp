#include "InWanderLand.h"
#include "HanselProjectileSkill.h"

const float throwingPieTimingFrame = 70.0f;

POD_HanselProjectileSkill HanselProjectileSkill::pod = POD_HanselProjectileSkill();
float HanselProjectileSkill::colliderEffectRatio = 1.0f;

coroutine::Coroutine HanselProjectileSkill::ThrowingPie(std::weak_ptr<HanselProjectileSkill> skill)
{
    float actualCollideRange = UrsulaBlindSkill::pod.skillScale * colliderEffectRatio;

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / pod.projectileSpeed };
    pieCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    pieObject = FBXPool::SingleInstance().Borrow("SM_Pie");
    pieObject.lock()->GetGameObject()->SetSelfActive(false);

    co_await std::suspend_always{};

    actualCollideRange = pod.pieScale;

    pieCollider.lock()->SetRadius(actualCollideRange);
    pieObject.lock()->GetTransform()->SetWorldScale({ pod.pieScale, pod.pieScale, pod.pieScale });
    pieCollider.lock()->GetTransform()->SetWorldRotation(direction);
    auto pieStartRotation = Quaternion::MakeWithForwardUp(direction.up * -1, direction);

    pieObject.lock()->GetTransform()->SetWorldRotation(pieStartRotation);
    //pieObject->GetTransform()->SetWorldRotation(pieObject->GetTransform()->GetWorldRotation().Up() * -1);
    float rotatePerFrame = 0.0f;

    while (forSeconds.Tick())
    {
        currentPos += direction * pod.projectileSpeed * Time::GetDeltaTime();
        pieCollider.lock()->GetTransform()->SetWorldPosition(currentPos + direction * pod.pieOffsetX + Vector3d(0, pod.pieOffsetY, 0));
        pieObject.lock()->GetTransform()->SetWorldPosition(currentPos + direction * pod.pieOffsetX + Vector3d(0, pod.pieOffsetY, 0 ));

        rotatePerFrame += pod.pieRotateSpeed * Time::GetDeltaTime();

        Vector3d directionPerFrame = (endPos - currentPos).Normalized();
		//pieObject->GetTransform()->SetWorldRotation(pieObject->GetTransform()->GetWorldRotation() + Vector3d(rotatePerFrame, 0, 0));
        auto euler =  pieObject.lock()->GetTransform()->GetLocalRotation().Euler();
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
            }
        }

        for (auto& each : pieCollider.lock()->GetFriends())
        {
            if (onceCollidedUnits.find(each) == onceCollidedUnits.end())
            {
                /// 충돌한 아군에게는 음향버프를 부여합니다.
                onceCollidedUnits.insert(each);
            }
        }
    }
   
    co_return;
}

float HanselProjectileSkill::GetCastRange()
{
    return pod.maxRange;
}

coroutine::Coroutine HanselProjectileSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
	auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
	auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    auto animator = owner.lock()->GetAnimator();

    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);

    coroutine::ForSeconds forThrowingSeconds{ pod.throwingStartDelay };

    owner.lock()->PlayAnimation(UnitAnimType::Throw, true);

    while (throwingPieTimingFrame >= animator.lock()->GetCurrentFrame())
    {
        co_await std::suspend_always{};
    }

    auto throwingCoroutine = owner.lock()->StartCoroutine(ThrowingPie(std::dynamic_pointer_cast<HanselProjectileSkill>(selfWeakPtr.lock())));
    throwingCoroutine.lock()->PushDestroyCallBack([this]()
        {
            UnitAcquisitionSphereColliderPool::SingleInstance().Return(pieCollider);
            FBXPool::SingleInstance().Return(pieObject);
            onceCollidedUnits.clear();
        });

    coroutine::ForSeconds forIdleSeconds{ 1.0f };
    while (forIdleSeconds.Tick()) { co_await std::suspend_always{}; }
    owner.lock()->PlayAnimation(UnitAnimType::Idle);
    co_yield coroutine::WaitForSeconds{ 0.3f };

	disableNavAgent.reset();
	blockFollowingNavigation.reset();
	owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    OnInterruption();
    co_return;
}

void HanselProjectileSkill::OnInterruption()
{

}
