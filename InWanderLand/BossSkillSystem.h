#pragma once
#include "SkillSystem.h"
#include "LocalTimeEntity.h"

class Unit;

/// <summary>
/// playerSkillSystem과는 다르게, 내부에서 자체적으로 사용할 스킬 및 위치를 정해주는 방식은 어떨까?
/// </summary>
class BossSkillSystem : public SkillSystem, public LocalTimeEntity
{
private:
	float m_elapsed{ 0.0f };
	float m_skillActivateDuration = 5.0f;

	float m_skillOneDuration = 3.0f;
	float m_skillTwoDuration = 3.0f;
	float m_skillFourDuration = 3.0f;

	float m_skillTwoColliderRange;

	int m_skillNum;

	Vector3d m_currentSkillPosition;

	Unit* currentSummonedDoorUnit;
	Unit* currentDerivedDoorUnit;

	SkillRequirements m_currentSkillRequirments{ nullptr };

	SkillRequirements m_skillOneRequirments;
	SkillRequirements m_skillTwoRequirments;
	SkillRequirements m_skillThreeRequirments;
	SkillRequirements m_skillFourRequirments;

	bool isBossSkill;
	bool isBossSkillActivating{ false };
	Unit::SkillEnum staticSelectedSkillNumber;
	Unit::SkillEnum currentSelectedSkillNumber;

public:
	virtual void ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) override;
	void ActivateSkillRandomly();
	void SelectSkillRandomly();

	void SetSkillOneRequirments(GameObject* p_obj, GameObject* p_debugObj);
	void SetSkillTwoRequirments(GameObject* p_obj, GameObject* p_debugObj);
	void SetSkillTwoRange(float p_range);
	void SetSkillFourRequirments(GameObject* p_obj, physics::SphereCollider* p_projectileCollider, GameObject* p_debugObj);

	void SelectSkill(Unit::SkillEnum p_enum);

	virtual void Start() override;
	virtual void Update() override;

	Unit::SkillEnum GetCurrentSelectedSkillNumber() const;

private:
	void ActivateSkillOne();
	void ActivateSkillTwo();
	void ActivateSkillThree();
	void ActivateSkillFour();
};

