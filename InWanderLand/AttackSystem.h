#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
/// <summary>
/// Unit의 AttackState에서 호출하는 함수를 다형적인 동작을 하게 만들기 위한 클래스.
/// </summary>

class AttackSystem : public Component
{
public:
	virtual void Attack(Unit* opponentUnit) = 0;
};

