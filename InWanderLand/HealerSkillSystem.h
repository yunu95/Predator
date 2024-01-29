#pragma once
#include "PlayerSkillSystem.h"

class Dotween;

class HealerSkillSystem : public PlayerSkillSystem
{
private:
	SkillRequirements QSkillFieldDamage;
	SkillRequirements WSkillFieldDamage;

	bool isQSkillActivating;
	bool isColliderSetActiveFalse;
	bool isQSkillColliderActivated;

	float QSkillJumpXdistance = 3.0f;
	float QSkillJumpYdistance = 2.0f;
	float reachTopTime = 1.5f;
	
	float QSkillColliderRemainTime = 1.0f;
	float QSkillColliderElapsed = 0.0f;

	bool isWSkillActivating;
	bool isWSkillColliderActivated;

	float WSkillColliderRemainTime = 1.0f;
	float WSkillColliderElapsed = 0.0f;

	Vector3d startPosition;

	void SetSkillRequirmentsActive(SkillRequirements p_requirments, bool p_boolen);


private:
	void CrushDown(int p_times);

public:
	void SetQSkillCollider(physics::SphereCollider* p_fieldDamageCollider);
	void SetQSkillObject(GameObject* p_fieldDamageObj);
	void SetQSkillDebugInfo(GameObject* p_fieldDebugObject);

	void SetWSkillCollider(physics::BoxCollider* p_fieldDamageCollider);
	void SetWSkillObject(GameObject* p_fieldDamageObj);
	void SetWSkillDebugInfo(GameObject* p_fieldDebugObject);

public:
	virtual void QSkillActivate(Vector3d skillPos) override;
	virtual void WSkillActivate(Vector3d skillPos) override;

public:
	virtual void Start() override;
	virtual void Update() override;
};

