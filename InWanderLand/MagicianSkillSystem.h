#pragma once
#include "PlayerSkillSystem.h"

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
	GameObject* m_unitTransformObject;

	SkillRequirements QSkillProjectile;
	SkillRequirements QSkillFieldDamage;

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
	void SetQSkillCollider(physics::SphereCollider* p_projectileCollider, physics::SphereCollider* p_fieldDamageColliderp);
	void SetQSkillObject(GameObject* p_projectileTransformObj, GameObject* p_fieldDamageTransformObj);
	void SetQSkillDebugPair(std::pair<GameObject*, float> p_projectileObjectPair, std::pair<GameObject*, float> p_fieldObjectPair);
	void SetUnitTransformObject(GameObject* obj);

public:
	virtual void Start() override;
	virtual void Update() override;
};

