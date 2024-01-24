#pragma once
#include "PlayerSkillSystem.h"

class WarriorSkillSystem : public PlayerSkillSystem
{
private:
	physics::SphereCollider* QSkillKnockBackCollider;
	physics::SphereCollider* WSkillCollider;

	bool isColliderSetActiveFalseSet;
	bool isQSkillActivating = false;

	GameObject* m_QSkillKnockBackDebugObject;	float m_QSkillRadius;
	GameObject* m_WSkillDebugObject;			float m_WSkillRadius;

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

	// parameter로 전달받는 radius는 실제 collider에 적용된 radius입니다.
	void SetKnockBackDebugObject(GameObject* obj, float radius);
	void SetWSkillDebugObject(GameObject* obj, float radius);

public:
	virtual void Start() override;
	virtual void Update() override;
};

