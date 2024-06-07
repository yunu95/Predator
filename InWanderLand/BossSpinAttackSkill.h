#pragma once
#include "Skill.h"

struct POD_BossSpinAttackSkill
{
	float skillPlayTime = 6.0f;
	float skillDamage = 8.0f;
	float colliderRadius = 10.0f;
	float knockBackDistance = 15.0f;
	float knockBackDuration = 1.0f;
	float paralyzingDuration = 2.0f;

	TO_JSON(POD_BossSpinAttackSkill)
	FROM_JSON(POD_BossSpinAttackSkill)
};

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

	std::shared_ptr<Reference::Guard> blockFollowingNavigation;
	std::shared_ptr<Reference::Guard> disableNavAgent;

	std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
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