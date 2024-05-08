#pragma once
#include "PlayerSkillSystem.h"

class WarriorSkillSystem : public PlayerSkillSystem
{
private:
	SkillRequirements QknockBackSkill;
	SkillRequirements WTauntSkill;

	bool isQSkillPreviewOn;
	bool isColliderSetActiveFalseSet;

	float m_QSkillRadius;
	float m_WSkillRadius;

	float m_QskillRushSpeed = 10.0f;
	float m_knockBackObjectRemainTime = 3.0f;

	float m_wSkillColliderRemainTime = 1.0f;

public:
	std::function<void()> m_developedFunctionToWSkill{ nullptr };

	virtual void ActivateSkillOne(Vector3d skillPos) override;
	virtual void ActivateSkillTwo(Vector3d skillPos) override;
	virtual void SetSkillRequirmentLocalTimeScale(float p_scale) override;

public:
	void SetQSkillKnockBackObject(GameObject* obj);
	void SetWSkillObject(GameObject* obj);

	// parameter로 전달받는 radius는 실제 collider에 적용된 radius입니다.
	void SetKnockBackDebugObject(GameObject* obj, float radius);
	void SetWSkillDebugObject(GameObject* obj, float radius);

public:
	virtual void Start() override;
	virtual void Update() override;
};

