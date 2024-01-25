#pragma once
#include "SkillSystem.h"

class Dotween;

/// <summary>
/// 스킬을 사용할 때, 미리 collider를 멤버로 Set해주고
/// 유닛의 상태를 제어하고 collider를 언제, 어디서 활성화/비활성화 할지 정해주는 컴포넌트.
/// </summary>
class PlayerSkillSystem : public SkillSystem
{
public:
	virtual void SkillActivate(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) override;

public:
	virtual void QSkillActivate(Vector3d skillPos) = 0;
	virtual void WSkillActivate(Vector3d skillPos) = 0;

protected:
	Dotween* m_UnitDotween;
	virtual void SetPlayerDotweenComponent(Dotween* p_dotween);
};

