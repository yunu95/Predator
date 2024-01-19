#pragma once
#include "PlayerSkillSystem.h"

class BlindFieldComponent;
class ParalysisFieldComponent;

class MagicianSkillSystem : public PlayerSkillSystem
{
private:
	struct SkillRequirements
	{
		physics::SphereCollider* skillCollider;
		GameObject* colliderObject;
		GameObject* debugObject;
		Dotween* dotweenComponent;
	};

	SkillRequirements QSkillProjectile;
	SkillRequirements QSkillFieldDamage;

	BlindFieldComponent* m_QSkillComponent;
	ParalysisFieldComponent* m_WSkillComponent;

	float m_QSkillProjectileSpeed = 10.0f;
	float m_QSkillFieldRemainTime = 4.0f;
	bool isQSkillActivating = false;
	
	float m_QSkillProjectileRadius;
	float m_QSkillFieldRadius;

	bool isColliderSetActiveFalseSet = false;

	void SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen);

public:
	virtual void QSkillActivate(Vector3d skillPos) override;
	virtual void WSkillActivate(Vector3d skillPos) override;
	virtual void ESkillActivate(Vector3d skillPos) override;
public:
	void SetInterActionComponent(BlindFieldComponent* p_QSkillComponent, ParalysisFieldComponent* p_WSkillComponent);
	void SetQSkillCollider(physics::SphereCollider* p_projectileCollider, physics::SphereCollider* p_fieldDamageColliderp);
	void SetQSkillObject(GameObject* p_projectileTransformObj, GameObject* p_fieldDamageTransformObj);
	void SetQSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair);

public:
	virtual void Start() override;
	virtual void Update() override;
};

