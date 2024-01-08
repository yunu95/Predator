#pragma once
#include "SkillSystem.h"
class WarriorSkillSystem : public SkillSystem
{
public:
	virtual void SkillActivate(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) override;

};

