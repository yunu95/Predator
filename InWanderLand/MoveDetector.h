#pragma once
#include "YunutyEngine.h"
#include <map>
#include <vector>
/// <summary>
/// 유닛의 자연스러운 움직임 구현을 위한 유닛 이동 감지 클래스.
/// 유닛이 움직일 때마다 유닛을 추적하고 있는 다른 객체에게 일러바치는 역할.
/// 
/// </summary>

class Unit;

class MoveDetector
{	
public:
	MoveDetector();
	static MoveDetector* instance;

public:
	static MoveDetector* GetInstance();

private:
	std::map<Unit*, std::vector<Unit*>> m_unitMap;				// First : Target	Second : Chasers

public:
	void SetTargetUnit(Unit* target);
	void SetChaserUnit(Unit* target, Unit* chaser);
	void TargetMove(Unit* target, Vector3d endPosition);					// target이 움직이면 해당 함수를 호출해준다.

};

