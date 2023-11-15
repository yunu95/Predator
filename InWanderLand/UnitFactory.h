#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 손쉽게 유닛을 만들 수 있도록 해주는 클래스.
/// 디폴트 값으로 unit 객체를 생성하고, 조정을 원한다면 GetComponent로 설정해준다.
/// </summary>

enum UnitType
{
	PLAYER,
	ENEMY,
};

class UnitFactory
{
public:
	UnitFactory();

private:
	yunuGI::Color playerColor;
	yunuGI::Color enemyColor;

	float defaultPlayerAtkRadius;
	float defaultPlayerIDRadius;

	float defaultEnemyAtkRadius;
	float defaultEnemyIDRadius;

public:
	yunutyEngine::GameObject* CreateUnit(UnitType unitType, yunutyEngine::NavigationField* navField);

};

