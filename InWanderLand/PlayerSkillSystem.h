#pragma once
#include "SkillSystem.h"

class Dotween;

/// <summary>
/// 스킬을 사용할 때, 미리 collider를 멤버로 Set해주고
/// 유닛의 상태를 제어하고 collider를 언제, 어디서 활성화/비활성화 할지 정해주는 컴포넌트.
/// </summary>
class PlayerSkillSystem : public SkillSystem
{
public:
	virtual void ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition);	
	virtual void RotateProjectile(GameObject* p_projectileObject, Vector3d p_endPosition);

	virtual void ActivateSkillOne(Vector3d skillPos) = 0;
	virtual void ActivateSkillTwo(Vector3d skillPos) = 0;

	virtual void Start() override;

	Vector3d m_currentSelectedSkillPosition;

	bool isSkillEnd = false;
};

