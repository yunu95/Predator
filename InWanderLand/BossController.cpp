#include "BossController.h"
#include "InWanderLand.h"


void BossController::OnContentsStop()
{
	currentState = 0;
	beforeSkillIndex = 0;
	EnemyController::OnContentsStop();
}

coroutine::Coroutine BossController::RoutineGlobal()
{
	int state = 0;
	auto boss = unitRoutines.begin()->first;
	while (true)
	{
		if (state == 0 && boss->GetUnitCurrentHp() / boss->GetUnitTemplateData().pod.max_Health <= 2 / 3)
		{
			state = 1;
			boss->OrderSkill(BossSummonMobSkill{}, Vector3d());
		}
		else if (state == 1 && boss->GetUnitCurrentHp() / boss->GetUnitTemplateData().pod.max_Health <= 1 / 3)
		{
			state = 2;
			boss->OrderSkill(BossSummonMobSkill{}, Vector3d());
		}
		co_await std::suspend_always();
	}
}

coroutine::Coroutine BossController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
	if (!unit.lock()->IsAlive())
	{
		co_return;
	}

	/// BossSummonMobSkill 이 발동 되었을 때에는 해당 루틴의 Timer 가 일시 정지가 되어야 할 것으로 보임

	/// Skill 을 사용할 때, 해당 Coroutine 의 타이머는 개별 동작하기에 Skill 완료되는 시점에 대한 것도
	/// 알아야 할 것으로 보임

	auto& gc = GlobalConstant::GetSingletonInstance().pod;

	switch (currentState)
	{
		case 0:
		{
			/// 시작
			currentState = 1;
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_4 };
			break;
		}
		case 1:
		{
			beforeSkillIndex = math::Random::GetRandomInt(1, 2);
			switch (beforeSkillIndex)
			{
				case 1:
				{
					unit.lock()->OrderSkill(BossSpinAttackSkill{}, Vector3d());
					break;
				}
				case 2:
				{
					/// 2번 스킬 사용
					break;
				}
				default:
					break;
			}
			currentState = 2;
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_1 };
			break;
		}
		case 2:
		{
			switch (beforeSkillIndex)
			{
				case 1:
				{
					/// 2번 스킬 사용
					break;
				}
				case 2:
				{
					unit.lock()->OrderSkill(BossSpinAttackSkill{}, Vector3d());
					break;
				}
				default:
					break;
			}
			beforeSkillIndex = 0;
			currentState = 3;
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_2 };
			break;
		}
		case 3:
		{
			auto randSkill = math::Random::GetRandomInt(1, 2);
			switch(randSkill)
			{
				case 1:
				{
					unit.lock()->OrderSkill(BossSpinAttackSkill{}, Vector3d());
					break;
				}
				case 2:
				{
					/// 2번 스킬 사용
					break;
				}
				default:
					break;
			}
			currentState = 4;
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_3 };
			break;
		}
		case 4:
		{
			unit.lock()->OrderSkill(BossSummonChessSkill{}, Vector3d());
			currentState = 0;
			co_yield coroutine::WaitForSeconds{ gc.bossSkillPeriod_4 };
			break;
		}
		default:
			break;
	}
}