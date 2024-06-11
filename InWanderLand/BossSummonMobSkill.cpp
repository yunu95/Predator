#include "InWanderLand.h"
#include "BossSummonMobSkill.h"
#include "LeftFrame.h"
#include "RightFrame.h"
#include "GlobalConstant.h"
#include "YunutyWaitForSeconds.h"

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
		leftFrame->OnBossAppear();
		rightFrame->OnBossAppear();
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
	owner.lock()->PlayAnimation(UnitAnimType::Skill3, true);
	auto animator = owner.lock()->GetAnimator();
	auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill3);
	coroutine::ForSeconds forSeconds{ anim->GetDuration() };

	if (rightFrame->HasChangedUnit())
	{
		leftFrame->ChangeUnit();
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

	OnInterruption();
	co_return;
}

void BossSummonMobSkill::OnInterruption()
{

}

coroutine::Coroutine BossSummonMobSkill::StartSummonTimer()
{
	while (true)
	{
		coroutine::ForSeconds forSeconds{ pod.summonPeriod };

		while (forSeconds.Tick())
		{
			co_await std::suspend_always{};
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
