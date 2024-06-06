#pragma once
#include "Skill.h"

struct POD_BossSpinAttackSkill
{
	float	skillRadius = 6.0f;
	float	skillDamage = 5.0f;
	float	skillPlayTime = 2.0f;
	float	knockBackPower = 5.0f;
	float	knckBackDuration = 0.6f;
	TO_JSON(POD_BossSpinAttackSkill)
	FROM_JSON(POD_BossSpinAttackSkill)
};

class UnitAcquisitionSphereCollider;
class BossSpinAttackSkill : public Skill
{
public:
	BossSpinAttackSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_One; }
	virtual coroutine::Coroutine operator()() override;
	virtual void OnInterruption() override;

	static POD_BossSpinAttackSkill pod;

private:
	std::unordered_set<Unit*> knockBackList;
	std::weak_ptr<UnitAcquisitionSphereCollider> knockBackCollider;
};




template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::BossSkill_One>(json& data)
{
	application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_BossSpinAttackSkill>>(BossSpinAttackSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::BossSkill_One>(json& data)
{
	application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_BossSpinAttackSkill>>(BossSpinAttackSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::BossSkill_One>(const json& data)
{
	application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_BossSpinAttackSkill>>(BossSpinAttackSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::BossSkill_One>(const json& data)
{
	application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_BossSpinAttackSkill>>(BossSpinAttackSkill::pod, data["POD"]);
	return true;
}