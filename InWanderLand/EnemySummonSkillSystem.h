#pragma once
#include "EnemySkillSystem.h"
#include "LocalTimeEntity.h"

class EnemySummonSkillSystem : public EnemySkillSystem, public LocalTimeEntity
{
private:
	float m_elapsed;
	float m_enemySpawnDuration;
	bool isThisUnitSummoned;

	Unit* m_currentSummonedUnit;

public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) override;

private:
	virtual void ActivateSkillOne() override;

};

