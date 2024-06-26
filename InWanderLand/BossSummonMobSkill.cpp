#include "InWanderLand.h"
#include "BossSummonMobSkill.h"
#include "LeftFrame.h"
#include "RightFrame.h"
#include "GlobalConstant.h"
#include "YunutyWaitForSeconds.h"
#include "VFXAnimator.h"
#include "Unit.h"

POD_BossSummonMobSkill BossSummonMobSkill::pod = POD_BossSummonMobSkill();

std::weak_ptr<coroutine::Coroutine> BossSummonMobSkill::summonCoroutine = std::weak_ptr<coroutine::Coroutine>();
BossSummon::LeftFrame* BossSummonMobSkill::leftFrame = nullptr;
BossSummon::RightFrame* BossSummonMobSkill::rightFrame = nullptr;

void BossSummonMobSkill::SetLeftFrame(BossSummon::LeftFrame* leftFrame)
{
	BossSummonMobSkill::leftFrame = leftFrame;
}

void BossSummonMobSkill::SetRightFrame(BossSummon::RightFrame* rightFrame)
{
	BossSummonMobSkill::rightFrame = rightFrame;
}

std::weak_ptr<Unit> BossSummonMobSkill::GetLeftFrameUnit()
{
	if (leftFrame == nullptr)
	{
		return std::weak_ptr<Unit>();
	}

	return leftFrame->unitFrame;
}

std::weak_ptr<Unit> BossSummonMobSkill::GetRightFrameUnit()
{
	if (rightFrame == nullptr)
	{
		return std::weak_ptr<Unit>();
	}

	return rightFrame->unitFrame;
}

void BossSummonMobSkill::OnBossAppear()
{
	if (leftFrame && rightFrame)
	{
		leftFrame->OnBossAppear();
		rightFrame->OnBossAppear();
	}
}

void BossSummonMobSkill::OnBossDie()
{
	if (leftFrame && rightFrame)
	{
		leftFrame->OnBossDie();
		rightFrame->OnBossDie();
	}
}

void BossSummonMobSkill::OnPause()
{
	if (!stepEffectAnimator.expired())
	{
		stepEffectAnimator.lock()->Pause();
	}
}

void BossSummonMobSkill::OnResume()
{
	if (!stepEffectAnimator.expired())
	{
		stepEffectAnimator.lock()->Resume();
	}
}

coroutine::Coroutine BossSummonMobSkill::operator()()
{
	if (leftFrame == nullptr || rightFrame == nullptr || leftFrame->HasChangedUnit())
	{
		co_return;
	}

	auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
	auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
	auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	auto rotRef = owner.lock()->referenceBlockRotation.Acquire();
	auto animator = owner.lock()->GetAnimator();

	owner.lock()->PlayAnimation(UnitAnimType::Skill3, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);

	effectCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(std::dynamic_pointer_cast<BossSummonMobSkill>(selfWeakPtr.lock())));
	effectCoroutine.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::Instance().Return(stepEffect);
		});
	auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill3);
	
	wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ owner, anim->GetDuration() };
	
	if (rightFrame->HasChangedUnit())
	{
		leftFrame->ChangeUnit();
		co_await std::suspend_always{};
		leftFrame->SummonUnit();
		if (!summonCoroutine.expired())
		{
			owner.lock()->DeleteCoroutine(summonCoroutine);
			owner.lock()->StartCoroutine(StartSummonTimer());
		}
	}
	else
	{
		rightFrame->ChangeUnit();
		co_await std::suspend_always{};
		rightFrame->SummonUnit();
		summonCoroutine = owner.lock()->StartCoroutine(StartSummonTimer());
	}

	while (forSeconds.Tick())
	{
		co_await std::suspend_always{};
	}

	disableNavAgent.reset();
	blockFollowingNavigation.reset();
	owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());

	co_return;
}

void BossSummonMobSkill::OnInterruption()
{
	if (!effectCoroutine.expired())
	{
		owner.lock()->DeleteCoroutine(effectCoroutine);
	}
}

coroutine::Coroutine BossSummonMobSkill::StartSummonTimer()
{
	while (true)
	{
		wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ BossController::Instance().GetBoss(), pod.summonPeriod};

		while (forSeconds.Tick())
		{
			co_await std::suspend_always{};
		}

		if (!leftFrame->IsAlive() && !rightFrame->IsAlive())
		{
			break;
		}
		
		bool summonUnit = false;
		while (!summonUnit)
		{
			auto randVal = math::Random::GetRandomInt(0, 1);
			switch (randVal)
			{
				case 0:
				{
					if (leftFrame->IsAlive())
					{
						leftFrame->SummonUnit();
						summonUnit = true;
					}
					break;
				}
				case 1:
				{
					if (rightFrame->IsAlive())
					{
						rightFrame->SummonUnit();
						summonUnit = true;
					}
					break;
				}
				default:
					break;
			}
		}
		co_await std::suspend_always{};
	}
	co_return;
}

coroutine::Coroutine BossSummonMobSkill::SpawningFieldEffect(std::weak_ptr<BossSummonMobSkill> skill)
{
	auto copyPtr = skill.lock();
	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();
	Vector3d endPos = startPos + deltaPos;
	Vector3d currentPos = startPos;

	stepEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Skill3_1");
	stepEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	stepEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
	stepEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(owner.lock()->GetTransform()->GetWorldScale());

	stepEffectAnimator = stepEffect.lock()->AcquireVFXAnimator();
	stepEffectAnimator.lock()->SetAutoActiveFalse();
	stepEffectAnimator.lock()->Init();
	stepEffectAnimator.lock()->Play();

	auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill3);

	wanderUtils::UnitCoroutine::ForSecondsFromUnit animSeconds{ skill.lock()->owner, anim->GetDuration() };

	while (animSeconds.Tick())
	{
		co_await std::suspend_always();
	}

	co_return;
}
