#include "InWanderLand.h"
#include "HanselProjectileSkill.h"

const float throwingPieTimingFrame = 70.0f;

POD_HanselProjectileSkill HanselProjectileSkill::pod = POD_HanselProjectileSkill();

coroutine::Coroutine HanselProjectileSkill::ThrowingPie()
{
    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / pod.projectileSpeed };
    auto pieCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    auto pieObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Pie");
    std::unordered_set<Unit*> onceCollidedUnits;
    co_await std::suspend_always{};

    pieCollider.lock()->SetRadius(pod.projectileRadius);
    pieObject->GetTransform()->SetWorldScale({ 3,3,3 });
    pieCollider.lock()->GetTransform()->SetWorldRotation(direction);
    auto pieStartRotation = Quaternion::MakeWithForwardUp(direction.up * -1, direction);

    pieObject->GetTransform()->SetWorldRotation(pieStartRotation);
    //pieObject->GetTransform()->SetWorldRotation(pieObject->GetTransform()->GetWorldRotation().Up() * -1);
    float rotatePerFrame = 0.0f;

    while (forSeconds.Tick())
    {
        currentPos += direction * pod.projectileSpeed * Time::GetDeltaTime();
        pieCollider.lock()->GetTransform()->SetWorldPosition(currentPos);
        pieObject->GetTransform()->SetWorldPosition(currentPos + Vector3d(0, pod.pieHeight, 0 ));

        rotatePerFrame += pod.pieRotateSpeed * Time::GetDeltaTime();

        Vector3d directionPerFrame = (endPos - currentPos).Normalized();
		//pieObject->GetTransform()->SetWorldRotation(pieObject->GetTransform()->GetWorldRotation() + Vector3d(rotatePerFrame, 0, 0));
        //pieObject->GetTransform()->SetWorldRotation(Quaternion::MakeAxisAngleQuaternion((pieObject->GetTransform()->GetWorldPosition() + directionPerFrame).right, rotatePerFrame));

        co_await std::suspend_always{};
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

    pieCollider.lock()->SetRadius(0.5);
    
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(pieCollider);
    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(pieObject);
    onceCollidedUnits.clear();
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
    owner.lock()->StartCoroutine(ThrowingPie());

    coroutine::ForSeconds forIdleSeconds{ 1.0f };
    while (forIdleSeconds.Tick()) { co_await std::suspend_always{}; }
    owner.lock()->PlayAnimation(UnitAnimType::Idle);
    co_yield coroutine::WaitForSeconds{ 0.3f };

	disableNavAgent.reset();
	blockFollowingNavigation.reset();
	owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());

    co_return;
}

void HanselProjectileSkill::OnInterruption()
{

}
