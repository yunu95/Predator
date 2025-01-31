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
	animator = owner.lock()->GetAnimator();
	skillAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill3);

	if (pod.skillDuration > 0)
	{
		skillSpeed = skillAnim->GetDuration() / pod.skillDuration;
	}

	owner.lock()->SetDefaultAnimation(UnitAnimType::None);
	animator.lock()->GetGI().SetPlaySpeed(skillSpeed);
	owner.lock()->PlayAnimation(UnitAnimType::Skill3);

	effectCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(std::dynamic_pointer_cast<BossSummonMobSkill>(selfWeakPtr.lock())));
	effectCoroutine.lock()->PushDestroyCallBack([this]()
		{
			animator.lock()->GetGI().SetPlaySpeed(1);
			stepEffectAnimator.lock()->SetSpeed(1);
			FBXPool::Instance().Return(stepEffect);
		});
		
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

	float localSkillDuration = 0;

	if (pod.skillDuration > 0)
	{
		localSkillDuration = pod.skillDuration;
	}
	else
	{
		localSkillDuration = skillAnim->GetDuration();
	}

	wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ owner, localSkillDuration };

	while (forSeconds.Tick())
	{
		co_await std::suspend_always{};
	}

	disableNavAgent.reset();
	blockFollowingNavigation.reset();
	owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
	if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
		owner.lock()->OrderAttackMove();
	animator.lock()->GetGI().SetPlaySpeed(1);

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
	stepEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d::one * owner.lock()->GetUnitTemplateData().pod.unit_scale);

	stepEffectAnimator = stepEffect.lock()->AcquireVFXAnimator();
	stepEffectAnimator.lock()->SetAutoActiveFalse();
	stepEffectAnimator.lock()->Init();
	stepEffectAnimator.lock()->Play();

	stepEffectAnimator.lock()->SetSpeed(skillSpeed);

	while (!stepEffectAnimator.lock()->IsDone())
	{
		co_await std::suspend_always();
	}

	co_return;
}
