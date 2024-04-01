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
		physics::Collider* skillCollider;
		GameObject* colliderObject;
		GameObject* debugObject;
		Dotween* dotweenComponent;
	};

	virtual void SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen);


};

