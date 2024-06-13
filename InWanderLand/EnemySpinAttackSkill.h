#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class ManagedFBX;
struct POD_EnemySpinAttackSkill
{
	float skillPlayTime = 6.0f;
	float skillCoolTime = 6.0f;
	float skillDamage = 8.0f;
	float skillRadius = 1.0f;
	float knockBackDistance = 15.0f;
	float knockBackDuration = 1.0f;
	float paralyzingDuration = 2.0f;
	float skillEndTimeAfterDamaged = 0.1f;

	TO_JSON(POD_EnemySpinAttackSkill)
	FROM_JSON(POD_EnemySpinAttackSkill)
};

class EnemySpinAttackSkill : public Skill
{
public:
	EnemySpinAttackSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_One; }
	virtual coroutine::Coroutine operator()() override;
	virtual void OnInterruption() override;

	static POD_EnemySpinAttackSkill pod;

private:
	coroutine::Coroutine SpawningSkillffect(std::weak_ptr<EnemySpinAttackSkill> skill);
	std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
	std::weak_ptr<ManagedFBX> chargeEffect;
	std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;
};




template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::EnemySpin>(json& data)
{
	application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_EnemySpinAttackSkill>>(EnemySpinAttackSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::EnemySpin>(json& data)
{
	application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_EnemySpinAttackSkill>>(EnemySpinAttackSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::EnemySpin>(const json& data)
{
	application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_EnemySpinAttackSkill>>(EnemySpinAttackSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::EnemySpin>(const json& data)
{
	application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_EnemySpinAttackSkill>>(EnemySpinAttackSkill::pod, data["POD"]);
	return true;
}