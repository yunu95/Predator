#include "BossController.h"
#include "InWanderLand.h"
#include "VFXAnimator.h"

#include "LeftFrame.h"
#include "RightFrame.h"

void BossController::RegisterUnit(std::weak_ptr<Unit> unit)
{
	boss = unit;
	EnemyController::RegisterUnit(unit);

	unit.lock()->onDamagedFromUnit.AddCallback([this](const std::weak_ptr<Unit>& target)
		{
			ChangeAttackTarget(target);
		}
	);

	unit.lock()->onDamaged.AddCallback([this]()
		{
			if (summonState == 0 && (boss.lock()->GetUnitCurrentHp() / boss.lock()->GetUnitMaxHp()) <= 2.0f / 3.0f)
			{
				summonState++;
				summonDone = false;
				boss.lock()->OrderSkill(BossSummonMobSkill{}, boss.lock()->GetTransform()->GetWorldPosition());
				boss.lock()->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([this]()
					{
						summonDone = true;
					});
				if (!unitRoutines.empty() && !unitRoutines.begin()->second.expired())
				{
					DeleteCoroutine(unitRoutines.begin()->second);
				}
			}
		}
	);

	unit.lock()->onDamaged.AddCallback([this]()
		{
			if (summonState == 1 && (boss.lock()->GetUnitCurrentHp() / boss.lock()->GetUnitMaxHp()) <= 1.0f / 3.0f)
			{
				summonState++;
				summonDone = false;
				boss.lock()->OrderSkill(BossSummonMobSkill{}, boss.lock()->GetTransform()->GetWorldPosition());
				boss.lock()->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([this]()
					{
						summonDone = true;
					});
				if (!unitRoutines.empty() && !unitRoutines.begin()->second.expired())
				{
					DeleteCoroutine(unitRoutines.begin()->second);
				}
			}
		}
	);

	unit.lock()->OnStateEngageCallback()[UnitBehaviourTree::Death].AddVolatileCallback([this]()
		{
			BossSummonMobSkill::OnBossDie();
			BossSummonChessSkill::OnBossDie();
			for (auto& each : onBossDieCallback)
			{
				each();
			}
		});

	unit.lock()->GetGameObject()->SetSelfActive(false);
}

void BossController::OnContentsStop()
{
	summonState = 0;
	summonDone = true;
	currentState = 0;
	beforeSkillIndex = 0;
	onBossDieCallback.clear();
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

void BossController::ChangeAttackTarget(const std::weak_ptr<Unit>& unit)
{
	if (unit.expired() || !unit.lock()->IsAlive())
	{
		return;
	}

	auto target = boss.lock()->GetAttackTarget();
	if (!target.expired() && target.lock()->IsAlive())
	{
		auto bossPos = boss.lock()->GetTransform()->GetWorldPosition();
		auto prevUnitPos = target.lock()->GetTransform()->GetWorldPosition();
		auto newUnitPos = unit.lock()->GetTransform()->GetWorldPosition();
		if ((newUnitPos - bossPos).MagnitudeSqr() < (prevUnitPos - bossPos).MagnitudeSqr())
		{
			boss.lock()->OrderAttack(unit);
		}
	}
}

coroutine::Coroutine BossController::BossAppearCoroutine()
{
	auto& gc = GlobalConstant::GetSingletonInstance().pod;

	auto pause = boss.lock()->referencePause.Acquire();
	auto blockFollowingNavigation = boss.lock()->referenceBlockFollowingNavAgent.Acquire();
	auto blockAnimLoop = boss.lock()->referenceBlockAnimLoop.Acquire();
	auto disableNavAgent = boss.lock()->referenceDisableNavAgent.Acquire();
	auto enableNavObstacle = boss.lock()->referenceEnableNavObstacle.Acquire();

	boss.lock()->GetGameObject()->SetSelfActive(true);
	boss.lock()->GetTransform()->SetWorldPosition(boss.lock()->GetTransform()->GetWorldPosition() + Vector3d(0, gc.bossAppearHeight, 0));
	co_await std::suspend_always();

	BossSummonMobSkill::OnBossAppear();

	wanderUtils::UnitCoroutine::ForSecondsFromUnit preAppear{ boss, gc.bossAppearTime };

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
	boss.lock()->PlayAnimation(UnitAnimType::Birth, Animation::PlayFlag_::None);
	appearEffectCorountine = boss.lock()->StartCoroutine(BossAppearEffectCoroutine());
	appearEffectCorountine.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::Instance().Return(appearEffect);
		});
	auto anim = wanderResources::GetAnimation(boss.lock()->GetFBXName(), UnitAnimType::Birth);
	wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ boss, anim->GetDuration() };

	while (forSeconds.Tick())
	{
		co_await std::suspend_always();
	}
	co_return;
}

coroutine::Coroutine BossController::BossAppearEffectCoroutine()
{
	appearEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Appear");
	appearEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(boss.lock()->GetTransform()->GetWorldPosition());
	appearEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(boss.lock()->GetTransform()->GetWorldRotation());
	auto appearEffectAnimator = appearEffect.lock()->AcquireVFXAnimator();
	appearEffectAnimator.lock()->SetAutoActiveFalse();
	appearEffectAnimator.lock()->Init();
	appearEffectAnimator.lock()->Play();
	
	while (!appearEffectAnimator.lock()->IsDone())
	{
		co_await std::suspend_always();
	}

	co_return;
}

coroutine::Coroutine BossController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
	if (unit.lock()->GetActive() && !unit.lock()->IsAlive())
	{
		co_return;
	}

	while (!summonDone)
	{
		co_await std::suspend_always();
	}

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
			wanderUtils::UnitCoroutine::ForSecondsFromUnit waitPeriod_4 { boss, gc.bossSkillPeriod_4 };

			while (waitPeriod_4.Tick())
			{
				co_await std::suspend_always();
			}

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
					unit.lock()->OrderSkill(BossImpaleSkill{}, unit.lock()->GetTransform()->GetWorldPosition() + skillDir);
					break;
				}
				default:
					break;
			}

			bool skillDone = false;
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([&]()
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
			wanderUtils::UnitCoroutine::ForSecondsFromUnit waitPeriod_1{ boss, gc.bossSkillPeriod_1 };

			while (waitPeriod_1.Tick())
			{
				co_await std::suspend_always();
			}

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
					unit.lock()->OrderSkill(BossImpaleSkill{}, unit.lock()->GetTransform()->GetWorldPosition() + skillDir);
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
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([&]()
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
			wanderUtils::UnitCoroutine::ForSecondsFromUnit waitPeriod_2{ boss, gc.bossSkillPeriod_2 };

			while (waitPeriod_2.Tick())
			{
				co_await std::suspend_always();
			}

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
					unit.lock()->OrderSkill(BossImpaleSkill{}, unit.lock()->GetTransform()->GetWorldPosition() + skillDir);
					break;
				}
				default:
					break;
			}

			bool skillDone = false;
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([&]()
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
			wanderUtils::UnitCoroutine::ForSecondsFromUnit waitPeriod_3{ boss, gc.bossSkillPeriod_3 };

			while (waitPeriod_3.Tick())
			{
				co_await std::suspend_always();
			}

			unit.lock()->OrderSkill(BossSummonChessSkill{}, unit.lock()->GetTransform()->GetWorldPosition());

			bool skillDone = false;
			unit.lock()->OnStateExitCallback()[UnitBehaviourTree::SkillOnGoing].AddVolatileCallback([&]()
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