#include "InWanderLand.h"
#include "RobinTauntSkill.h"
#include "VFXAnimator.h"

POD_RobinTauntSkill RobinTauntSkill::pod = POD_RobinTauntSkill();

const float damageTimingFrame = 24.0f;

float RobinTauntSkill::colliderEffectRatio = 6.0f * 0.5f;

coroutine::Coroutine RobinTauntSkill::SpawningSkillffect(std::weak_ptr<RobinTauntSkill> skill)
{
	float actualCollideRange = RobinTauntSkill::pod.skillRadius * (1 / colliderEffectRatio);

	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();

	tauntEffect = FBXPool::SingleInstance().Borrow("VFX_Robin_Skill2");
	tauntEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	tauntEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(actualCollideRange, actualCollideRange, actualCollideRange));
	tauntEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
	auto chargeEffectAnimator = tauntEffect.lock()->AcquireVFXAnimator();
	chargeEffectAnimator.lock()->SetAutoActiveFalse();
	chargeEffectAnimator.lock()->Init();

	auto animator = owner.lock()->GetAnimator();
	auto tauntAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Taunt);

	tauntCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
	tauntCollider.lock()->SetRadius(pod.skillRadius);
	tauntCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

	coroutine::ForSeconds forSeconds{ pod.skillPlayTime };
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
					each->OrderAttack(owner);
					each->Damaged(owner, pod.skillDamage);
				}
			}
		}
	}
	co_await std::suspend_always{};

	while (!chargeEffectAnimator.lock()->IsDone())
	{
		co_await std::suspend_always{};
	}
	co_return;
}

coroutine::Coroutine RobinTauntSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Taunt, true);
	effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(std::dynamic_pointer_cast<RobinTauntSkill>(selfWeakPtr.lock())));
	effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
		{
			tauntCollider.lock()->SetRadius(0.5);
			UnitAcquisitionSphereColliderPool::Instance().Return(tauntCollider);
			FBXPool::SingleInstance().Return(tauntEffect);
		});

	co_yield coroutine::WaitForSeconds(pod.skillPlayTime);

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
