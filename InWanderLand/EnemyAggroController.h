#pragma once
#include "YunutyEngine.h"
#include "EnemyController.h"

// 성난 군중은 플레이어 유닛들을 향해 거의 어택땅만 합니다.
// 플레이어 하나에 유닛들이 과하게 몰리는 현상을 막기 위해, 일정시간을 주기로 적들은 공격목표를 다시 골고루 분배합니다.
class EnemyAggroController : public EnemyController, public SingletonComponent<EnemyAggroController>
{
public:
    // 전역적으로 반복되는 루틴
    virtual coroutine::Coroutine RoutineGlobal() override;
    // 개별 유닛들에게서 반복되는 루틴
    //virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit);
private:
    std::array<int, PlayerCharacterType::Num> IdealAggros = std::array<int, PlayerCharacterType::Num>();
    std::array<int, PlayerCharacterType::Num> currentAggros = std::array<int, PlayerCharacterType::Num>();
};