#pragma once
#include "EnemySkillSystem.h"

class Unit;

/// <summary>
/// playerSkillSystem과는 다르게, 내부에서 자체적으로 사용할 스킬 및 위치를 정해주는 방식은 어떨까?
/// </summary>
class BossSkillSystem : public EnemySkillSystem
{
private:
	float m_skillUsageDuration{ 0.0f };
	float m_elapsed{ 0.0f };

	float m_skillOneDuration = 3.0f;
	float m_skillTwoDuration = 3.0f;
	float m_skillFourDuration = 3.0f;

	float m_skillTwoColliderRange;

	int m_skillNum;

	Unit* m_warriorUnit;
	Unit* m_magicianUnit;
	Unit* m_healerUnit;

	Unit* currentSummonedDoorUnit;
	Unit* currentDerivedDoorUnit;

	SkillRequirements m_skillOneRequirments;
	SkillRequirements m_skillTwoRequirments;
	SkillRequirements m_skillThreeRequirments;
	SkillRequirements m_skillFourRequirments;

public:
	void ActivateSkillRandomly();

	void SetSkillOneRequirments(GameObject* p_obj, physics::SphereCollider* p_projectileCollider, GameObject* p_debugObj);
	void SetSkillTwoRequirments(GameObject* p_obj, physics::BoxCollider* p_projectileCollider, GameObject* p_debugObj);
	void SetSkillTwoRange(float p_range);
	void SetSkillFourRequirments(GameObject* p_obj, physics::SphereCollider* p_projectileCollider, GameObject* p_debugObj);

	virtual void Start() override;
	virtual void Update() override;

private:
	void ActivateSkill(Unit::SkillEnum p_currentSkill);
	virtual void ActivateSkillOne() override;
	void ActivateSkillTwo();
	void ActivateSkillThree();
	void ActivateSkillFour();
};

