#include "InWanderLand.h"
#include "UrsulaParalysisSkill.h"
#include "VFXAnimator.h"

#include <math.h>


POD_UrsulaParalysisSkill UrsulaParalysisSkill::pod = POD_UrsulaParalysisSkill();
float UrsulaParalysisSkill::colliderEffectRatio = 3.0f * 0.5f;

coroutine::Coroutine UrsulaParalysisSkill::SpawningFieldEffect(std::weak_ptr<UrsulaParalysisSkill> skill)
{
	auto animator = owner.lock()->GetAnimator();

	float actualCollideRange = pod.skillRadius * (1 / colliderEffectRatio);

	damageCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
	damageCollider.lock()->SetRadius(pod.skillRadius);
	damageCollider.lock()->GetTransform()->SetWorldPosition(targetPos);
	knockBackCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
	knockBackCollider.lock()->SetRadius(pod.skillRadius);
	knockBackCollider.lock()->GetTransform()->SetWorldPosition(targetPos);

	tentacleObject = FBXPool::SingleInstance().Borrow("SVFX_Ursula_Skill2_Tentacle");
	waveObject = FBXPool::SingleInstance().Borrow("SVFX_Ursula_Skill2_Wave");
	tentacleObject.lock()->GetTransform()->SetWorldScale({ actualCollideRange, actualCollideRange, actualCollideRange });
	waveObject.lock()->GetTransform()->SetWorldScale({ actualCollideRange, actualCollideRange, actualCollideRange });

	auto tentacleAnimator = tentacleObject.lock()->GetGameObject()->GetChildren()[0]->GetComponent<yunutyEngine::graphics::Animator>();
	auto waveAnimator = waveObject.lock()->GetGameObject()->GetChildren()[0]->GetComponent<yunutyEngine::graphics::Animator>();

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
		else if (each->GetName() == L"root|Ani_SVFX_Wave")
		{
			waveAnimation = each;
			waveAnimator->PushAnimation(waveAnimation);
		}
	}

	waveObject.lock()->GetGameObject()->GetChildren()[0]->AddComponent<VFXAnimator>()->SetAutoActiveFalse();

	tentacleObject.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos);
	waveObject.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos);

	tentacleAnimator->Play(tentacleAnimation);
	waveAnimator->Play(waveAnimation);

	co_await std::suspend_always{};

	bool hit = false;
	while (!hit)
	{
		auto curFrame = tentacleAnimator->GetCurrentFrame();

		if (curFrame >= 45)
		{
			hit = true;
		}

		if (hit)
		{
			co_await std::suspend_always{};
			for (auto& each : damageCollider.lock()->GetEnemies())
			{
				each->Damaged(owner, pod.skillDamage);
			}
		}
		co_await std::suspend_always{};
	}

	bool knockBacked = false;
	while (!knockBacked)
	{
		auto curFrame = tentacleAnimator->GetCurrentFrame();

		if (curFrame >= 80)
		{
			knockBacked = true;
		}

		if (knockBacked)
		{
			co_await std::suspend_always{};
			for (auto& each : knockBackCollider.lock()->GetEnemies())
			{
				each->KnockBack(targetPos, pod.knockBackDuration);
			}
		}
		co_await std::suspend_always{};
	}

	while (!waveAnimator->IsDone())
	{
		co_await std::suspend_always{};
	}

    co_return;
}

coroutine::Coroutine UrsulaParalysisSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	effectColliderCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(dynamic_pointer_cast<UrsulaParalysisSkill>(selfWeakPtr.lock())));
	effectColliderCoroutine.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::SingleInstance().Return(tentacleObject);
			FBXPool::SingleInstance().Return(waveObject);
			UnitAcquisitionSphereColliderPool::SingleInstance().Return(damageCollider);
			UnitAcquisitionSphereColliderPool::SingleInstance().Return(knockBackCollider);
		});
    owner.lock()->PlayAnimation(UnitAnimType::Skill2, true);
    auto animator = owner.lock()->GetAnimator();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill2);
    coroutine::ForSeconds forSeconds{ anim->GetDuration() };
    
    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};
    }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    co_return;
}

void UrsulaParalysisSkill::OnInterruption()
{
	//if (!effectColliderCoroutine.expired())
	//{
	//	owner.lock()->DeleteCoroutine(effectColliderCoroutine);
	//}
}
