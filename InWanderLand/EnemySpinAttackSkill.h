#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class ManagedFBX;
struct POD_EnemySpinAttackSkill
{
	float foreswingDuration = -1.0f;
	float skillDuration = -1.0f;
	float backswingDuration = -1.0f;

	float skillCoolTime = 6.0f;
	float skillDamage = 8.0f;
	float skillRadius = 1.0f;
	float knockBackDistance = 15.0f;
	float knockBackDuration = 1.0f;

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

	virtual void OnPause() override;
	virtual void OnResume() override;

	static POD_EnemySpinAttackSkill pod;

private:
	coroutine::Coroutine SpawningSkillffect(std::weak_ptr<EnemySpinAttackSkill> skill);
	std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
	std::weak_ptr<ManagedFBX> chargeEffect;
	std::weak_ptr<ManagedFBX> previewEffect;
	std::weak_ptr<yunutyEngine::graphics::Animator> animator;
	std::weak_ptr<VFXAnimator> previewEffectAnimator;
	std::weak_ptr<VFXAnimator> chargeEffectAnimator;
	std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;

	float foreswingSpeed = 1.f;
	float skillSpeed = 1.f;
	float backswingSpeed = 1.f;
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