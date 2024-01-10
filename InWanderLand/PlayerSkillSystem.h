#pragma once
#include "SkillSystem.h"

class Dotween;

class PlayerSkillSystem : public SkillSystem
{
public:
	virtual void SkillActivate(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) override;

public:
	virtual void QSkillActivate(Vector3d skillPos) = 0;
	virtual void WSkillActivate(Vector3d skillPos) = 0;
	virtual void ESkillActivate(Vector3d skillPos) = 0;

protected:
	Dotween* m_UnitDotween;
	virtual void SetPlayerDotweenComponent(Dotween* p_dotween);
};

