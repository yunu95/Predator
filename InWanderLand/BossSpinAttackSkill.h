#pragma once
#include "Skill.h"

struct POD_BossSpinAttackSkill
{
	float foreswingDuration = -1.0f;
	float skillDuration = -1.0f;
	float backswingDuration = -1.0f;

	float skillDamage = 8.0f;
	float skillRadius = 5.0f;
	float knockBackDistance = 15.0f;
	float knockBackDuration = 1.0f;

	TO_JSON(POD_BossSpinAttackSkill)
	FROM_JSON(POD_BossSpinAttackSkill)
};

class BossSpinAttackSkill : public Skill
{
public:
	BossSpinAttackSkill() = default;
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_One; }
	virtual coroutine::Coroutine operator()() override;
	virtual void OnInterruption() override;

	virtual void OnPause() override;
	virtual void OnResume() override;

	static POD_BossSpinAttackSkill pod;

private:
	coroutine::Coroutine SpawningSkillffect(std::weak_ptr<BossSpinAttackSkill> skill);
	std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
	std::weak_ptr<ManagedFBX> chargeEffect;
	std::weak_ptr<ManagedFBX> previewEffect;
	std::weak_ptr<yunutyEngine::graphics::Animator> animator;
	std::weak_ptr<VFXAnimator> previewEffectAnimator;
	std::weak_ptr<VFXAnimator> chargeEffectAnimator;
	std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;

	float colliderEffectRatio;
	float foreswingSpeed = 1.f;
	float skillSpeed = 1.f;
	float backswingSpeed = 1.f;
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