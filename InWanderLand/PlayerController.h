#pragma once
#include "YunutyEngine.h"
#include <map>
/// <summary>
/// UnitFactory에서 Player를 만들 경우, PlayerController에 해당 Player를 등록한다.
/// 고유 번호를 배분하여 키입력으로 지정할 수 있도록 해보자!
/// </summary>

class RTSCam;
class Unit;

class PlayerController
{
private:
	PlayerController();
	~PlayerController();
	static PlayerController* instance;

public:
	static PlayerController* GetInstance();

private:
	RTSCam* m_movingSystemComponent;
	std::map<Unit*, int> playerComponentMap;
	
public:
	void MakeLeftClickMove(int unitSerialNumber);
	void MakeLeftClickAttackMove(int unitSerialNumber);

	void QSkillKeyPressed(int unitSerialNumber);

	void SetMovingSystemComponent(RTSCam* sys);
	void AddPlayerUnit(Unit* p_playerUnit); 
};

