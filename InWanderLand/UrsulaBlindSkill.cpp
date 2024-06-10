#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"
#include "VFXAnimator.h"

#include <math.h>

Vector3d UrsulaBlindSkill::skillStart = Vector3d();
Vector3d UrsulaBlindSkill::skillDestination = Vector3d();

POD_UrsulaBlindSkill UrsulaBlindSkill::pod = POD_UrsulaBlindSkill();

float UrsulaBlindSkill::colliderEffectRatio = 3.0f;

coroutine::Coroutine UrsulaBlindSkill::SpawningFieldEffect()
{
	auto onUrsulaPosEffect = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_1");
	auto onTargetPosEffect1 = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_2");
	auto onTargetPosEffect2 = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_2");
	auto onTargetPosEffect3 = FBXPool::SingleInstance().Borrow("VFX_Ursula_Skill1_2");

	UpdatePosition(owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition(), targetPos);

	onUrsulaPosEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
	onTargetPosEffect1.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Top(skillDestination));
	onTargetPosEffect2.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Left(skillDestination));
	onTargetPosEffect3.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Right(skillDestination));

	onUrsulaPosEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.skillScale, pod.skillScale, pod.skillScale));
	onTargetPosEffect1.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.skillScale, pod.skillScale, pod.skillScale));
	onTargetPosEffect2.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.skillScale, pod.skillScale, pod.skillScale));
	onTargetPosEffect3.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.skillScale, pod.skillScale, pod.skillScale));

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

	co_await std::suspend_always{};

	while (!onTargetPosAnimator1.lock()->IsDone())
	{
		co_await std::suspend_always{};
	}

	FBXPool::SingleInstance().Return(onUrsulaPosEffect);
	FBXPool::SingleInstance().Return(onTargetPosEffect1);
	FBXPool::SingleInstance().Return(onTargetPosEffect2);
	FBXPool::SingleInstance().Return(onTargetPosEffect3);

	co_return;
}

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
	float actualCollideRange = UrsulaBlindSkill::pod.skillScale * colliderEffectRatio;

	auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
	auto animator = owner.lock()->GetAnimator();
	auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill1);

	UpdatePosition(owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition(), targetPos);

	circle_Top = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
	circle_Top.lock()->SetRadius(actualCollideRange);
	circle_Left = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
	circle_Left.lock()->SetRadius(actualCollideRange);
	circle_Right = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
	circle_Right.lock()->SetRadius(actualCollideRange);

	circle_Top.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Top(skillDestination));
	circle_Left.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Left(skillDestination));
	circle_Right.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Right(skillDestination));

	owner.lock()->StartCoroutine(SpawningFieldEffect());

	coroutine::ForSeconds forSeconds{ anim->GetDuration() };

	int hitCount = 0;
	while (forSeconds.Tick())
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

			/// 장판 3개의 콜라이더와 이펙트를 생성하는 코루틴 함수 호출.

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

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    OnInterruption();
    co_return;
}

void UrsulaBlindSkill::OnInterruption()
{
    circle_Top.lock()->SetRadius(0.5);
    circle_Left.lock()->SetRadius(0.5);
    circle_Right.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Top);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Left);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Right);
}

void UrsulaBlindSkill::UpdatePosition(const Vector3d& start, const Vector3d& dest)
{
    skillStart = start;
    skillDestination = dest;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Top(const Vector3d& dest)
{
	float actualCollideRange = UrsulaBlindSkill::pod.skillScale * colliderEffectRatio;

    auto length = actualCollideRange * 2 + pod.skillOffset;
    auto skillDir = (dest - skillStart).Normalized();
    return dest + std::sqrt(3) / 3 * skillDir * length;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Left(const Vector3d& dest)
{
	float actualCollideRange = UrsulaBlindSkill::pod.skillScale * colliderEffectRatio;

    auto length = actualCollideRange * 2 + pod.skillOffset;
    auto skillDir = (dest - skillStart).Normalized();
    auto left = Vector3d::Cross(skillDir, Vector3d::up).Normalized();
    return dest - std::sqrt(3) / 6 * skillDir * length + left * length / 2;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Right(const Vector3d& dest)
{
	float actualCollideRange = UrsulaBlindSkill::pod.skillScale * colliderEffectRatio;

    auto length = actualCollideRange * 2 + pod.skillOffset;
    auto skillDir = (dest - skillStart).Normalized();
    auto right = Vector3d::Cross(-skillDir, Vector3d::up).Normalized();
    return dest - std::sqrt(3) / 6 * skillDir * length + right * length / 2;
}


