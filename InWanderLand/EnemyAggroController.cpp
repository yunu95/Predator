#include "EnemyAggroController.h"
#include "InWanderLand.h"

using namespace yunutyEngine::math;

// 일정시간마다 적 유닛들을 플레이어에게 골고루 분배한다.
coroutine::Coroutine EnemyAggroController::RoutineGlobal()
{
	const auto& gc = GlobalConstant::GetSingletonInstance().pod;
	int aggroUpdateQuota = gc.enemyAggroUpdateQuota + Random::GetRandomFloat(gc.enemyAggroUpdateQuotaNoise);
	int aggroUpdateCount = 0;
	auto idealProp = PlayerController::Instance().GetAggroProportions();
	IdealAggros[PlayerCharacterType::Ursula] = ceil(idealProp[PlayerCharacterType::Ursula] * unitRoutines.size());
	IdealAggros[PlayerCharacterType::Hansel] = ceil(idealProp[PlayerCharacterType::Hansel] * unitRoutines.size());
	IdealAggros[PlayerCharacterType::Robin] = unitRoutines.size() - idealProp[PlayerCharacterType::Ursula] - idealProp[PlayerCharacterType::Hansel];
	currentAggros.fill(0);

	// 어그로가 어떻게 분포되어 있는지 먼저 확인한다.
	for (auto& [unit, routine] : unitRoutines)
	{
		if (!unit->GetAttackTarget().expired())
		{
			currentAggros[unit->GetAttackTarget().lock()->GetUnitTemplateData().pod.playerUnitType.enumValue]++;
		}
	}

	// 어그로를 분산시킨다.
	std::array<bool, PlayerCharacterType::Num> tauntTarget = std::array<bool, PlayerCharacterType::Num>();
	auto tauntUpdate = [&](std::array<bool, PlayerCharacterType::Num>& arr) 
		{
			for (int i = 0; i < PlayerCharacterType::Num; i++)
			{
				if (currentAggros[i] < IdealAggros[i])
				{
					arr[i] = true;
				}
			}
		};

	auto coroStop = [&]()
		{
			bool startChange = false;
			bool allTrue = true;
			for (int i = 0; i < PlayerCharacterType::Num; i++)
			{
				startChange |= tauntTarget[i];
				allTrue &= tauntTarget[i];
			}
			return !startChange || allTrue;
		};

	tauntUpdate(tauntTarget);
	if (coroStop())
	{
		co_yield coroutine::WaitForSeconds{ gc.enemyAggroUpdateInterval + Random::GetRandomFloat(gc.enemyAggroUpdateIntervalNoise) };
		co_return;
	}

	for (auto& [unit, routine] : unitRoutines)
	{
		if (aggroUpdateCount >= aggroUpdateQuota)
		{
			break;
		}

		tauntUpdate(tauntTarget);
		if (coroStop())
		{
			break;
		}

		if (!unit->GetAttackTarget().expired() && !tauntTarget[unit->GetAttackTarget().lock()->GetUnitTemplateData().pod.playerUnitType.enumValue])
		{
			int playerIndex = Random::GetRandomInt(0, PlayerCharacterType::Num - 1);
			while (!tauntTarget[playerIndex])
			{
				playerIndex = Random::GetRandomInt(0, PlayerCharacterType::Num - 1);
			}

			unit->OrderAttack(PlayerController::Instance().GetPlayers().at(playerIndex));
			aggroUpdateCount++;
		}
	}
	co_yield coroutine::WaitForSeconds{ gc.enemyAggroUpdateInterval + Random::GetRandomFloat(gc.enemyAggroUpdateIntervalNoise) };
	co_return;
}
