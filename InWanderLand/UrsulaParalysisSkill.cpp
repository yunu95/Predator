#include "InWanderLand.h"
#include "UrsulaParalysisSkill.h"
#include "VFXAnimator.h"

#include <math.h>


POD_UrsulaParalysisSkill UrsulaParalysisSkill::pod = POD_UrsulaParalysisSkill();

coroutine::Coroutine UrsulaParalysisSkill::SpawningFieldEffect()
{
	auto animator = owner.lock()->GetAnimator();

	auto tentacleObject = FBXPool::SingleInstance().Borrow("SVFX_Ursula_Skill2_Tentacle");
	auto waveObject = FBXPool::SingleInstance().Borrow("SVFX_Ursula_Skill2_Wave");
	tentacleObject.lock()->GetTransform()->SetWorldScale({ 3,3,3 });
	waveObject.lock()->GetTransform()->SetWorldScale({ 3,3,3 });

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


	skillCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
	skillCollider.lock()->SetRadius(pod.skillRadius);
	skillCollider.lock()->GetTransform()->SetWorldPosition(targetPos);

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
			for (auto& each : skillCollider.lock()->GetEnemies())
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
			for (auto& each : skillCollider.lock()->GetEnemies())
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

	FBXPool::SingleInstance().Return(tentacleObject);
	FBXPool::SingleInstance().Return(waveObject);

    co_return;
}

coroutine::Coroutine UrsulaParalysisSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	owner.lock()->StartCoroutine(SpawningFieldEffect());
    owner.lock()->PlayAnimation(UnitAnimType::Skill2, true);
    auto animator = owner.lock()->GetAnimator();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill2);
    coroutine::ForSeconds forSeconds{ anim->GetDuration() };
    
    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};
    }

//     /// 마지막으로 경직
//     for (auto& each : skillCollider.lock()->GetEnemies())
//     {
//         each->Paralyze(pod.skillParalysisTime);
//     }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());

    OnInterruption();
    co_return;
}

void UrsulaParalysisSkill::OnInterruption()
{
    skillCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(skillCollider);
}
