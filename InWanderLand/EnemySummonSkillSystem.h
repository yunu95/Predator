#pragma once
#include "EnemySkillSystem.h"

class EnemySummonSkillSystem : public EnemySkillSystem
{
private:
	float m_elapsed;
	float m_enemySpawnDuration;
	bool isThisUnitSummoned;

	Unit* m_currentSummonedUnit;

public:
	virtual void Start() override;
	virtual void Update() override;

private:
	virtual void ActivateSkillOne() override;

};

