#pragma once
#include "YunutyEngine.h"
#include <map>
#include "Unit.h"
/// <summary>
/// UnitFactory에서 Player를 만들 경우, PlayerController에 해당 Player를 등록한다.
/// 고유 번호를 배분하여 키입력으로 지정할 수 있도록 해보자!
/// </summary>

class RTSCam;
class Unit;

class PlayerController
{
public:
	enum class OrderType
	{
		Move,
		AttackMove,
		QSkill,
	};

private:
	PlayerController();
	~PlayerController();
	static PlayerController* instance;

public:
	static PlayerController* GetInstance();

private:
	RTSCam* m_movingSystemComponent;
	std::unordered_map<Unit::UnitType, Unit*> playerComponentMap;
	Unit::UnitType currentSelectedSerialNumber;
	int previousSerialNumber = 0;

public:
	float lookRotationDuration = 0.1f;

	//void SelectFunctionByOrderType(int unitSerialNumber, OrderType p_orderType);

public:
	//void ApplyCurrentPlayerOrder(int unitSerialNumber, OrderType orderType);
	void SetMovingSystemComponent(RTSCam* sys);
	void AddPlayerUnit(Unit* p_playerUnit);

	void SetLeftClickMove();
	void SetLeftClickAttackMove();
	void SetLeftClickSkill(Unit::SkillEnum p_skillNum);

	void SetLeftClickEmpty();
	void SetRightClickEmpty();

	void SetCurrentPlayerSerialNumber(Unit::UnitType p_num);

	std::unordered_map<Unit::UnitType, Unit*> GetPlayerMap() const;
	Unit* FindSelectedUnitByUnitType(Unit::UnitType p_type);

};

