#include "InWanderLand.h"
#include "RobinChargeSkill.h"
#include "VFXAnimator.h"
#include "BossSummonMobSkill.h"

POD_RobinChargeSkill RobinChargeSkill::pod = POD_RobinChargeSkill();

coroutine::Coroutine RobinChargeSkill::operator()()
{
	auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
	auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
	auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	auto blockRot = owner.lock()->referenceBlockRotation.Acquire();
	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();
	if (deltaPos.Magnitude() > GetMaxDistance())
	{
		deltaPos = direction * GetMaxDistance();
	}
	Vector3d endPos = startPos + deltaPos;
	Vector3d currentPos = startPos;

	auto animator = owner.lock()->GetAnimator();
	rushAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Rush);
	slamAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Slam);

	if (pod.foreswingDuration > 0)
	{
		foreswingSpeed = rushAnim->GetDuration() / pod.foreswingDuration;
	}

	/// 1. 디폴트 애니메이션을 None으로 설정.
	owner.lock()->SetDefaultAnimation(UnitAnimType::None);
	animator.lock()->GetGI().SetPlaySpeed(foreswingSpeed);
	owner.lock()->PlayAnimation(UnitAnimType::Rush, Animation::PlayFlag_::Blending);

	
	coroutine::ForSeconds foreforSeconds{ rushAnim->GetDuration() / foreswingSpeed };
	
	auto startRotation = owner.lock()->GetTransform()->GetWorldRotation();
	auto desiredRotation = Quaternion::MakeWithForwardUp(direction, direction.up);

	while (foreforSeconds.Tick())
	{
		owner.lock()->GetTransform()->SetWorldRotation(Quaternion::Lerp(startRotation, desiredRotation, foreforSeconds.ElapsedNormalized()));
		co_await std::suspend_always{};
	}

	animator.lock()->Pause();

	co_await std::suspend_always{};

	coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / pod.rushSpeed };
	knockbackCollider = UnitAcquisitionSphereColliderPool::Instance().Borrow(owner.lock());
	knockbackCollider.lock()->SetRadius(pod.rushKnockbackRadius);
	bool isAnimChangedToSlam = false;

	while (forSeconds.Tick())
	{
		currentPos += direction * pod.rushSpeed * Time::GetDeltaTime();
		knockbackCollider.lock()->GetTransform()->SetWorldPosition(currentPos);
		owner.lock()->GetTransform()->SetWorldPosition(currentPos);
		co_await std::suspend_always{};
		for (auto& each : knockbackCollider.lock()->GetEnemies())
		{
			Vector3d delta = pod.rushKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
			each->Damaged(owner, GetDamageRush());
			if (each != BossSummonMobSkill::GetLeftFrameUnit().lock().get() && each != BossSummonMobSkill::GetRightFrameUnit().lock().get())
			{
				SFXManager::PlaySoundfile3D(wanderResources::GetSoundPath(EffectSoundType::Enum::HitSkill_Robin_Q_RushHit), each->GetTransform()->GetWorldPosition());
				each->KnockBackRelativeVector(delta, pod.rushKnockbackDuration);
			}
		}

		if (forSeconds.ElapsedNormalized() >= 0.9f && !isAnimChangedToSlam)
		{
			if (pod.backswingDuration > 0)
			{
				backswingSpeed = slamAnim->GetDuration() / pod.backswingDuration;
			}

			animator.lock()->GetGI().SetPlaySpeed(backswingSpeed);
			owner.lock()->PlayAnimation(UnitAnimType::Slam, Animation::PlayFlag_::Blending);
			animator.lock()->Resume();
			isAnimChangedToSlam = true;
		}
	}
	//owner.lock()->PlayAnimation(UnitAnimType::Slam, Animation::PlayFlag_::Blending);
	//animator.lock()->Resume();
	knockbackCollider.lock()->SetRadius(pod.impactKnockbackRadius);

	auto effectCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(dynamic_pointer_cast<RobinChargeSkill>(selfWeakPtr.lock()), startPos));
	effectCoroutine.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::Instance().Return(chargeEffect);
			UnitAcquisitionSphereColliderPool::Instance().Return(knockbackCollider);
			if (!chargeEffectAnimator.expired())
			{
				chargeEffectAnimator.lock()->SetSpeed(1);
			}
		});

	co_await std::suspend_always{};
	for (auto& each : knockbackCollider.lock()->GetEnemies())
	{
		Vector3d delta = pod.impactKnockbackDistance * (each->GetTransform()->GetWorldPosition() - currentPos).Normalized();
		//each->Paralyze(pod.impactStunDuration);
		each->Damaged(owner, GetDamageImpact());
		if (BossSummonMobSkill::GetRightFrameUnit().expired() || (each != BossSummonMobSkill::GetLeftFrameUnit().lock().get() && each != BossSummonMobSkill::GetRightFrameUnit().lock().get()))
		{
			each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, pod.impactKnockbackDuration);
		}
	}
	RTSCam::Instance().ApplyShake(pod.impactCamShakeDistance, pod.impactCamShakeFrequency, pod.impactCamShakeDecreaseFactor, endPos);

	while (animator.lock()->GetCurrentAnimation() == slamAnim && !animator.lock()->IsDone())
	{
		co_await std::suspend_always{};
	}

	//co_yield coroutine::WaitForSeconds(slamAnim->GetDuration() - 0.01f);
	//animator.lock()->Resume();
	disableNavAgent.reset();
	blockFollowingNavigation.reset();
	owner.lock()->Relocate(currentPos);
	//owner.lock()->PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending);

	/// 2. 스킬 완전 종료 이전에 해당 로직 삽입.
	if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
		owner.lock()->OrderAttackMove();

	animator.lock()->GetGI().SetPlaySpeed(1);

	owner.lock()->SetRotation(owner.lock()->GetTransform()->GetWorldRotation() * Quaternion(Vector3d(0, 180, 0)), 0);

	co_return;
}

void RobinChargeSkill::OnInterruption()
{
	auto animator = owner.lock()->GetAnimator();
	animator.lock()->Resume();
	animator.lock()->GetGI().SetPlaySpeed(1);
	if (!chargeEffectAnimator.expired())
		chargeEffectAnimator.lock()->SetSpeed(1);
	owner.lock()->SetRotation(owner.lock()->GetTransform()->GetWorldRotation() * Quaternion(Vector3d(0, 180, 0)), 0);
}

float RobinChargeSkill::GetMaxDistance()
{
	return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_Q_RANGE) ? pod.maxDistanceUpgraded : pod.maxDistance;
}

float RobinChargeSkill::GetDamageRush()
{
	return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_Q_DAMAGE) ? pod.damageRushUpgraded : pod.damageRush;
}

float RobinChargeSkill::GetDamageImpact()
{
	return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::ROBIN_Q_DAMAGE) ? pod.damageImpactUpgraded : pod.damageImpact;
}

coroutine::Coroutine RobinChargeSkill::SpawningSkillffect(std::weak_ptr<RobinChargeSkill> skill, Vector3d skillStartPos)
{
	auto persistance = skill.lock();

	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - skillStartPos;
	Vector3d direction = deltaPos.Normalized();

	chargeEffect = FBXPool::Instance().Borrow("VFX_Robin_Skill1");

	chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
	chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.effectScale, pod.effectScale, pod.effectScale));

	chargeEffectAnimator = chargeEffect.lock()->AcquireVFXAnimator();
	chargeEffectAnimator.lock()->SetAutoActiveFalse();
	chargeEffectAnimator.lock()->Init();

	if (pod.backswingDuration > 0)
	{
		effectSpeed = chargeEffectAnimator.lock()->GetDuration() / pod.backswingDuration;
	}

	chargeEffectAnimator.lock()->SetSpeed(effectSpeed);
	chargeEffectAnimator.lock()->Play();

	co_await std::suspend_always{};

	while (!chargeEffectAnimator.lock()->IsDone())
	{
		co_await std::suspend_always{};
	}

	co_return;
}