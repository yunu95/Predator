#pragma once
#include "SkillDevelopmentSystem.h"

class Unit;
class BleedingComponent;
class TauntingComponent;

class RobinSkillDevelopmentSystem : public SkillDevelopmentSystem, public SingletonComponent<RobinSkillDevelopmentSystem>
{
private:
	BleedingComponent* m_passiveComponent;
	TauntingComponent* m_tauntingComponent;

public:
	virtual void Start() override;
	void SetRobinPassiveComponent(BleedingComponent* p_passive);

	friend Unit;
};

