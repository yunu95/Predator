#pragma once
#include "PlayerSkillSystem.h"

class BlindFieldComponent;
class ParalysisFieldComponent;

class MagicianSkillSystem : public PlayerSkillSystem
{
private:
	SkillRequirements QSkillProjectile;
	SkillRequirements QSkillFieldDamage;

	SkillRequirements WSkillProjectile;
	SkillRequirements WSkillFieldDamage;

	BlindFieldComponent* m_QSkillComponent;
	ParalysisFieldComponent* m_WSkillComponent;

	float m_QSkillProjectileSpeed = 10.0f;
	float m_QSkillFieldRemainTime = 4.0f;
	bool isQSkillActivating = false;
	
	float m_QSkillProjectileRadius;
	float m_QSkillFieldRadius;

	float m_WSkillProjectileSpeed = 20.0f;
	float m_WSkillFieldRemainTime = 3.0f;
	bool isWSkillActivating = false;

	float m_WSkillProjectileRadius;
	float m_WSkillFieldRadius;

	bool isColliderSetActiveFalseSet = false;

public:
	virtual void ActivateSkillOne(Vector3d skillPos) override;
	virtual void ActivateSkillTwo(Vector3d skillPos) override;

public:
	void SetInterActionComponent(BlindFieldComponent* p_QSkillComponent, ParalysisFieldComponent* p_WSkillComponent);
	
	void SetQSkillObject(GameObject* p_projectileObj, GameObject* p_fieldDamageObj);
	void SetQSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair);

	void SetWSkillObject(GameObject* p_projectileTransformObj, GameObject* p_fieldDamageTransformObj);
	void SetWSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair);

public:
	virtual void Start() override;
	virtual void Update() override;
};

