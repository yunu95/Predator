#pragma once
#include "PlayerSkillSystem.h"

class WarriorSkillSystem : public PlayerSkillSystem
{
private:
	physics::SphereCollider* QSkillKnockBackCollider;
	physics::SphereCollider* WSkillCollider;

	bool isColliderSetActiveFalseSet;

	GameObject* m_unitTransformObject;

	float m_QskillRushSpeed = 10.0f;
	float m_knockBackObjectRemainTime = 1.0f;

	float m_wSkillColliderRemainTime = 1.0f;

public:
	virtual void QSkillActivate(Vector3d skillPos) override;
	virtual void WSkillActivate(Vector3d skillPos) override;
	virtual void ESkillActivate(Vector3d skillPos) override;

public:
	void SetQSkillKnockBackCollider(physics::SphereCollider* p_collider);
	void SetWSkillCollider(physics::SphereCollider* p_collider);

	void SetUnitTransformObject(GameObject* obj);

	virtual void Start() override;
	virtual void Update() override;
};

