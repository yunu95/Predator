#include "BossController.h"
#include "InWanderLand.h"

#include "LeftFrame.h"
#include "RightFrame.h"

void BossController::RegisterUnit(std::weak_ptr<Unit> unit)
{
	boss = unit;
	EnemyController::RegisterUnit(unit);

	unit.lock()->onDamaged.AddCallback([this]()
		{
			if (summonState == 0 && (boss.lock()->GetUnitCurrentHp() / boss.lock()->GetUnitTemplateData().pod.max_Health) <= 2 / 3)
			{
				summonState++;
				boss.lock()->OrderSkill(BossSummonMobSkill{}, boss.lock()->GetTransform()->GetWorldPosition());
				if (!unitRoutines.empty())
				{
					for (auto [target, coro] : unitRoutines)
					{
						DeleteCoroutine(coro);
					}
					unitRoutines.clear();
				}
			}
		}
	);

	unit.lock()->onDamaged.AddCallback([this]()
		{
			if (summonState == 1 && (boss.lock()->GetUnitCurrentHp() / boss.lock()->GetUnitTemplateData().pod.max_Health) <= 1 / 3)
			{
				summonState++;
				boss.lock()->OrderSkill(BossSummonMobSkill{}, boss.lock()->GetTransform()->GetWorldPosition());
				if (!unitRoutines.empty())
				{
					for (auto [target, coro] : unitRoutines)
					{
						DeleteCoroutine(coro);
					}
					unitRoutines.clear();
				}
			}
		}
	);

	unit.lock()->OnStateEngageCallback()[UnitBehaviourTree::Death].AddVolatileCallback([]()
		{
			BossSummonMobSkill::OnBossDie();
			BossSummonChessSkill::OnBossDie();
		});

	unit.lock()->GetGameObject()->SetSelfActive(false);
}

void BossController::OnContentsStop()
{
	summonState = 0;
	currentState = 0;
	beforeSkillIndex = 0;
	EnemyController::OnContentsStop();
}

void BossController::BossAppear()
{
	if (!boss.expired())
	{
		StartCoroutine(BossAppearCoroutine());
	}
}

std::weak_ptr<Unit> BossController::GetBoss()
{
	return boss;
}

coroutine::Coroutine BossController::BossAppearCoroutine()
{
	auto& gc = GlobalConstant::GetSingletonInstance().pod;

	auto pause = boss.lock()->referencePause.Acquire();
	auto blockFollowingNavigation = boss.lock()->referenceBlockFollowingNavAgent.Acquire();
	auto blockAnimLoop = boss.lock()->referenceBlockAnimLoop.Acquire();
	auto disableNavAgent = boss.lock()->referenceDisableNavAgent.Acquire();

	boss.lock()->GetGameObject()->SetSelfActive(true);
	boss.lock()->GetTransform()->SetWorldPosition(boss.lock()->GetTransform()->GetWorldPosition() + Vector3d(0, gc.bossAppearHeight, 0));
	co_await std::suspend_always();

	BossSummonMobSkill::OnBossAppear();

	coroutine::ForSeconds preAppear{ gc.bossAppearTime };
	auto initVel = wanderUtils::GetInitSpeedOfFreeFall(gc.bossAppearTime, Vector3d(0, gc.bossAppearHeight, 0), Vector3d(0, 0.5, 0));
	while (preAppear.Tick())
	{
		initVel += Vector3d::down * gc.gravitySpeed * preAppear.Elapsed();
		boss.lock()->GetTransform()->SetWorldPosition(boss.lock()->GetTransform()->GetWorldPosition() + initVel * preAppear.Elapsed());
		auto curPos = boss.lock()->GetTransform()->GetWorldPosition();
		if (curPos.y < 0.5)
		{
			curPos.y = 0.5;
			boss.lock()->GetTransform()->SetWorldPosition(curPos);
			break;
		}
		co_await std::suspend_always();
	}

	boss.lock()->GetTransform()->SetWorldPosition(boss.lock()->GetTransform()->GetWorldPosition() - Vector3d(0, 0.5, 0));
	boss.lock()->PlayAnimation(UnitAnimType::Birth, false);
	auto animator = boss.lock()->GetAnimator();
	auto anim = wanderResources::GetAnimation(boss.lock()->GetFBXName(), UnitAnimType::Birth);
	coroutine::ForSeconds forSeconds{ anim->GetDuration() };

	while (forSeconds.Tick())
	{
		co_await std::suspend_always();
	}
	co_return;
}

coroutine::Coroutine BossController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
	if (!unit.lock()->IsAlive())
	{
		co_return;
	}

	/// BossSummonMobSkill 이 발동 되었을 때에는 해당 루틴의 Timer 가 일시 정지가 되어야 할 것으로 보임

	auto& gc = GlobalConstant::GetSingletonInstance().pod;

	switch (currentState)
	{
		case 0:
		{
			/// 시작
			currentState = 1;
			break;
		}
		case 1:
		{
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_4 };

			beforeSkillIndex = math::Random::GetRandomInt(1, 2);
			switch (beforeSkillIndex)
			{
				case 1:
				{
					unit.lock()->OrderSkill(BossSpinAttackSkill{}, unit.lock()->GetTransform()->GetWorldPosition());
					break;
				}
				case 2:
				{
					Vector3d skillDir = Vector3d::zero;
					std::weak_ptr<Unit> targetUnit = unit.lock()->GetAttackTarget();
					if (targetUnit.expired() || !targetUnit.lock()->IsAlive())
					{
						for (auto pUnit : PlayerController::Instance().GetPlayers())
						{
							if (pUnit.expired() || !pUnit.lock()->IsAlive())
							{
								continue;
							}
							targetUnit = pUnit;
							break;
						}
					}
					skillDir = (targetUnit.lock()->GetTransform()->GetWorldPosition() - unit.lock()->GetTransform()->GetWorldPosition()).Normalized();
					unit.lock()->OrderSkill(BossImpaleSkill{}, targetUnit.lock()->GetTransform()->GetWorldPosition() + skillDir);
					break;
				}
				default:
					break;
			}

			bool skillDone = false;
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::Skill].AddVolatileCallback([&]()
				{
					skillDone = true;
				});
			while (!skillDone)
			{
				co_await std::suspend_always();
			}
			currentState = 2;
			break;
		}
		case 2:
		{
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_1 };

			switch (beforeSkillIndex)
			{
				case 1:
				{
					Vector3d skillDir = Vector3d::zero;
					std::weak_ptr<Unit> targetUnit = unit.lock()->GetAttackTarget();
					if (targetUnit.expired() || !targetUnit.lock()->IsAlive())
					{
						for (auto pUnit : PlayerController::Instance().GetPlayers())
						{
							if (pUnit.expired() || !pUnit.lock()->IsAlive())
							{
								continue;
							}
							targetUnit = pUnit;
							break;
						}
					}
					skillDir = (targetUnit.lock()->GetTransform()->GetWorldPosition() - unit.lock()->GetTransform()->GetWorldPosition()).Normalized();
					unit.lock()->OrderSkill(BossImpaleSkill{}, targetUnit.lock()->GetTransform()->GetWorldPosition() + skillDir);
					break;
				}
				case 2:
				{
					unit.lock()->OrderSkill(BossSpinAttackSkill{}, unit.lock()->GetTransform()->GetWorldPosition());
					break;
				}
				default:
					break;
			}

			bool skillDone = false;
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::Skill].AddVolatileCallback([&]()
				{
					skillDone = true;
				});
			while (!skillDone)
			{
				co_await std::suspend_always();
			}
			beforeSkillIndex = 0;
			currentState = 3;
			break;
		}
		case 3:
		{
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_2 };

			auto randSkill = math::Random::GetRandomInt(1, 2);
			switch(randSkill)
			{
				case 1:
				{
					unit.lock()->OrderSkill(BossSpinAttackSkill{}, unit.lock()->GetTransform()->GetWorldPosition());
					break;
				}
				case 2:
				{
					Vector3d skillDir = Vector3d::zero;
					std::weak_ptr<Unit> targetUnit = unit.lock()->GetAttackTarget();
					if (targetUnit.expired() || !targetUnit.lock()->IsAlive())
					{
						for (auto pUnit : PlayerController::Instance().GetPlayers())
						{
							if (pUnit.expired() || !pUnit.lock()->IsAlive())
							{
								continue;
							}
							targetUnit = pUnit;
							break;
						}
					}
					skillDir = (targetUnit.lock()->GetTransform()->GetWorldPosition() - unit.lock()->GetTransform()->GetWorldPosition()).Normalized();
					unit.lock()->OrderSkill(BossImpaleSkill{}, targetUnit.lock()->GetTransform()->GetWorldPosition() + skillDir);
					break;
				}
				default:
					break;
			}

			bool skillDone = false;
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::Skill].AddVolatileCallback([&]()
				{
					skillDone = true;
				});
			while (!skillDone)
			{
				co_await std::suspend_always();
			}
			currentState = 4;
			break;
		}
		case 4:
		{
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_3 };

			unit.lock()->OrderSkill(BossSummonChessSkill{}, unit.lock()->GetTransform()->GetWorldPosition());

			bool skillDone = false;
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::Skill].AddVolatileCallback([&]()
				{
					skillDone = true;
				});
			while (!skillDone)
			{
				co_await std::suspend_always();
			}
			currentState = 1;
			break;
		}
		default:
			break;
	}
}