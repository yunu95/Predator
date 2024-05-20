#pragma once
#include "Unit.h"

/// <summary>
/// 스킬을 사용하는 유닛들의 클래스.
/// PlayerUnit은 해당 클래스를 상속 받고,
/// Elite Monster의 경우엔 해당 클래스의 객체가 될 예정. Trap Unit 들도 현재는 SkillUnit으로 분류
/// </summary>
class SkillUnit : public Unit
{
public:
	virtual void Start() override;

protected:
	void SkillEngage();
	void SkillUpdate();
	
public:
	bool isSkillUsed{ false };

	void RegisterSkillWithAnimation(SkillEnum p_enum);
};

