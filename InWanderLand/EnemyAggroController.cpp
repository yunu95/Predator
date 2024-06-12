#include "EnemyAggroController.h"
#include "InWanderLand.h"

using namespace yunutyEngine::math;

// 일정시간마다 적 유닛들을 플레이어에게 골고루 분배한다.
coroutine::Coroutine EnemyAggroController::RoutineGlobal()
{
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    int aggroUpdateQuota = gc.enemyAggroUpdateQuota + Random::GetRandomFloat(gc.enemyAggroUpdateQuotaNoise);
    int aggroUpdateCount = 0;
    IdealAggros = PlayerController::Instance().GetAggroProportions();
    IdealAggros[PlayerCharacterType::Robin] *= unitRoutines.size();
    IdealAggros[PlayerCharacterType::Ursula] *= unitRoutines.size();
    IdealAggros[PlayerCharacterType::Hansel] *= unitRoutines.size();
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
    for (auto& [unit, routine] : unitRoutines)
    {
        if (unit->GetAttackTarget().expired() ||
            (currentAggros[unit->GetAttackTarget().lock()->GetUnitTemplateData().pod.playerUnitType.enumValue] >
                IdealAggros[unit->GetAttackTarget().lock()->GetUnitTemplateData().pod.playerUnitType.enumValue]
                ))
        {
            int randomOffset = Random::GetRandomInt(1, PlayerCharacterType::Num);
            bool aggroChanged = false;
            for (int i = 0; i < PlayerCharacterType::Num; i++)
            {
                int playerIndex = (i + randomOffset) % PlayerCharacterType::Num;
                if (aggroChanged = currentAggros[playerIndex] < IdealAggros[playerIndex]
                    && PlayerController::Instance().GetPlayers().at(playerIndex).lock()->IsAlive())
                {
                    if (!unit->GetAttackTarget().expired())
                    {
                        currentAggros[unit->GetAttackTarget().lock()->GetUnitTemplateData().pod.playerUnitType.enumValue]--;
                    }
                    currentAggros[playerIndex]++;
                    unit->OrderAttack(PlayerController::Instance().GetPlayers().at(playerIndex));
                    aggroUpdateCount++;
                    break;
                }
            }
            if (aggroUpdateCount >= aggroUpdateQuota)
                break;
        }
    }
    co_yield coroutine::WaitForSeconds{ gc.enemyAggroUpdateInterval + Random::GetRandomFloat(gc.enemyAggroUpdateIntervalNoise) };
    co_return;
}
