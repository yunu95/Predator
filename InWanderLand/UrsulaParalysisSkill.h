#pragma once
#include "Skill.h"

struct POD_UrsulaParalysisSkill
{
    float foreswingDuration = -1.0f;
    float skillDuration = -1.0f;

    float skillCoolTime = 2.0f;
    float skillCost = 10.0f;
    float skillRange = 6.f;
    float skillRadius = 1.f;
    float skillRadiusUpgraded = 1.f;
    float skillDamage = 5.f;
    float skillDamageUpgraded = 5.f;
    float skillParalysisTime = 3.0f;
    float knockBackDuration = 2.0f;

    float   zoomFactor = 1.0f;
    TO_JSON(POD_UrsulaParalysisSkill)
        FROM_JSON(POD_UrsulaParalysisSkill)
};

class UrsulaParalysisSkill : public Skill
{
public:
    UrsulaParalysisSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::URSULA_W; }
    virtual float GetCastRange() override { return pod.skillRange; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;
    static float GetSkillRadius();
    static float GetSkillDamage();

    static POD_UrsulaParalysisSkill pod;

private:
    coroutine::Coroutine SpawningFieldEffect(std::weak_ptr<UrsulaParalysisSkill> skill);

    std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;
    std::weak_ptr<UnitAcquisitionSphereCollider> damageCollider;
    std::weak_ptr<UnitAcquisitionSphereCollider> knockBackCollider;
    std::weak_ptr<ManagedFBX> tentacleObject;
    std::weak_ptr<ManagedFBX> waveObject;
    std::weak_ptr<VFXAnimator> waveVFXAnimator;
    std::weak_ptr<yunutyEngine::graphics::Animator> animator;
    yunutyEngine::graphics::Animator* tentacleAnimator = nullptr;
    yunutyEngine::graphics::Animator* waveAnimator = nullptr;

    yunuGI::IAnimation* paralysisAnim = nullptr;

    float colliderEffectRatio;
    float foreswingSpeed = 1.f;
    float skillSpeed = 1.f;
};






template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::URSULA_W>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_UrsulaParalysisSkill>>(UrsulaParalysisSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::URSULA_W>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_UrsulaParalysisSkill>>(UrsulaParalysisSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::URSULA_W>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_UrsulaParalysisSkill>>(UrsulaParalysisSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::URSULA_W>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_UrsulaParalysisSkill>>(UrsulaParalysisSkill::pod, data["POD"]);
    return true;
}
