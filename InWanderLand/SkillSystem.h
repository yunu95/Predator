#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
/// <summary>
/// Unit이 SkillState에서 호출해주는 함수는 SkillSystem에서 구현한다.
/// 플레이어 유닛별로 클래스를 만들어 parameter로 Unit::SkillEnum를 받아 동작하게 한다.
/// </summary>

class Dotween;

class SkillSystem : public Component
{
protected:
	Unit* m_unitComponent;
	Dotween* m_unitDotween;
	NavigationAgent* m_unitNavComponent;

protected:
	virtual void SetOtherComponentsAsMember();

	struct SkillRequirements
	{
		GameObject* colliderObject;
		GameObject* debugObject;
		Dotween* dotweenComponent;
	};

	virtual void SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen);
	virtual void SetSkillRequirmentsPosition(SkillRequirements p_requirments, Vector3d p_pos);
	virtual void SetSkillRequirmentsRotation(SkillRequirements p_requirments, Quaternion p_quat);
public:
	virtual void ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) = 0;
};

