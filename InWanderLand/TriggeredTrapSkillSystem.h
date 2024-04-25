#pragma once
#include "EnemySkillSystem.h"

class TriggeredTrapSkillSystem : public EnemySkillSystem
{
private:
	SkillRequirements m_trapRequirment;

public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) override;

	void SetSkillRequirment(GameObject* p_colliderObj, GameObject* p_debugObj);

	virtual void ActivateSkillOne() override;
};

