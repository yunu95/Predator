#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
/// <summary>
/// Unit이 SkillState에서 호출해주는 함수는 SkillSystem에서 구현한다.
/// 플레이어 유닛별로 클래스를 만들어 parameter로 Unit::SkillEnum를 받아 동작하게 한다.
/// </summary>
class SkillSystem : public Component
{
protected:
	float m_totalSkillTime = 0.0f;	// 해당 시간 후에 Unit의 상태를 Idle로 돌이켜줍니다.
public:
	virtual void SkillActivate(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) = 0;
};

