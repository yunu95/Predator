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

class TacticModeSystem : public SingletonClass<TacticModeSystem>
{
public:
	enum OrderType
	{
		Move,
		AttackMove,
		QSkill,
		WSkill
	};

	void SetCurrentSelectedPlayerUnit(Unit::UnitType p_type);

	void SetLeftClickAddQueueForAttackMove(InputManager::SelectedSerialNumber currentSelectedNum);
	void SetLeftClickAddQueueForSkill(InputManager::SelectedSerialNumber currentSelectedNum, Unit::SkillEnum currentSelectedSkill);

	void EngageTacticMode();
	void ExitTacticMode();
	void SetMovingSystemComponent(RTSCam* sys);
	bool IsTacticModeActivated(Unit* p_unit);			/// 전술모드가 끝날 때, parameter의 유닛이 입력된 명령이 있는가를 판별합니다. fsm transition에서 사용. 

	void CallQueueFunction(Unit* p_unit);

	bool isTacticModeStarted;

private:
	int queueOrderIndex = 0;
	int executingOrderIndex = 0;
	RTSCam* m_rtsCam;

	Unit::UnitType m_currentSelectedPlayerNumber;
	Unit* currentSelectedUnit;
	Unit* currentActivatedUnit;

	std::unordered_map<Unit::UnitType, Unit*> playerComponentMap;
	std::unordered_map<int, Unit*> processingUnitMap;						// queue에 쌓인 유닛의 스킬을 순서대로 처리하기 위한 map.
	std::multimap<Unit*, Vector3d> processingSkillPosMap;						

	std::queue<std::function<void()>>* currentSelectedQueue;

	std::queue<std::function<void()>> warriorQueue;
	std::queue<std::function<void()>> magicianQueue;
	std::queue<std::function<void()>> healerQueue;

	void SetCurrentSelectedQueue(Unit* p_currentUnit);
};

