#pragma once
#include "YunutyEngine.h"
#include "InputManager.h"
#include "Unit.h"
#include <unordered_map>

/// <summary>
/// 전술모드 시스템.
/// 활성화 시 TimeScale(0.0f), queue에 스킬 등록.
/// 확정 시 queue 순서대로 스킬 순차적 사용.
/// 돌진 스킬은 등속 운동... 시간을 어떻게 가져올 것인가 -> 좋은 로직은 시간을 가져오는 것이 아닌, 스킬이 끝날 때 알려주는 것.
/// 투사체 스킬은 던지는 모션이 끝나면 스킬이 끝나는 것으로 간주.
/// </summary>

class RTSCam;
class CursorDetector;

class TacticModeSystem : public SingletonComponent<TacticModeSystem>, public Component
{
public:
    virtual void OnEnable() override;
    virtual void Start() override;
    virtual void Update() override;

    enum OrderType
    {
        Move,
        AttackMove,
        QSkill,
        WSkill
    };

    void SetTacticModeRightClickFunction(InputManager::SelectedSerialNumber currentSelectedNum);
    void SetLeftClickAddQueueForAttackMove(InputManager::SelectedSerialNumber currentSelectedNum);
    void SetLeftClickAddQueueForSkill(InputManager::SelectedSerialNumber currentSelectedNum, Unit::SkillEnum currentSelectedSkill);

    /// Tutorial 관련 멤버
    void RegisterTutorialQuestAction(Unit::UnitType p_targetUnit, OrderType p_orderType);

    void EngageTacticMode();
    void ExitTacticMode();
    void SetMovingSystemComponent(RTSCam* sys);

    bool IsOrderingTimingNow() const;
    bool IsUnitsPerformingCommand() const;
    bool IsTacticModeCoolTime() const;

    void SetCurrentCoolTimeElapsed(float p_duration);
    float GetLeftCoolTime();

    void SetCurrentGauge(int p_gauge);

    void ReportTacticActionFinished();

    CursorDetector* m_cursorDetector;

private:
    int m_maxGauge{ 10 };
    int m_currentGauge{ 0 };
    float m_gaugeIncreaseDuration{ 3.0f };
    float m_gaugeIncreaseElapsed{ 0.0f };

    bool isCoolTime{ false };
    float m_engageCoolTimeDuration{ 5.0f };
    float m_engageCoolTimeElapsed{ 0.0f };

    bool isTacticModeOperating;					// 명령을 내리는 시간일 때 true.
    bool isTacticOrderPerforming;				// 내린 명령을 수행하고 있을 때 true.

    RTSCam* m_rtsCam;

    Unit* currentSelectedUnit{ nullptr };

    std::unordered_map<Unit::UnitType, Unit*> playerComponentMap;

    std::queue<Unit*> sequenceQueue;
};

