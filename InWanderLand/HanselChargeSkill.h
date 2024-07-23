#pragma once
#include "Skill.h"

struct POD_HanselChargeSkill
{
    float   foreswingDuration = -1.0f;
    float	skillDuration = -1.0f;
    float   backswingDuration = -1.0f;
    float   jumpSpeed = 1.0f;

    float	coolTime = 2.0f;
    float	skillCost = 10.0f;
    float	maxRange = 10.0f;
    float	maxRangeUpgraded = 10.0f;
    float	maxJumpHeight = 5.0f;
    float   damage = 10.0f;
    float   damageUpgraded = 10.0f;
    float   skillRadius = 10.0f;
    float impactKnockbackDistance = 8.0f;
    float impactKnockbackDuration = 1.0f;
    float impactCamShakeDistance1 = 0.5f;
    float impactCamShakeFrequency1 = 10.0f;
    float impactCamShakeDecreaseFactor1 = 0.4f;
    float impactCamShakeDistance2 = 0.5f;
    float impactCamShakeFrequency2 = 10.0f;
    float impactCamShakeDecreaseFactor2 = 0.4f;

    TO_JSON(POD_HanselChargeSkill)
        FROM_JSON(POD_HanselChargeSkill)
};

class HanselChargeSkill : public Skill
{
public:
    HanselChargeSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::HANSEL_Q; }
    virtual float GetCastRange() override { return HanselChargeSkill::GetMaxRange(); }
    virtual coroutine::Coroutine operator()()override;
    //virtual bool ShouldTargetOnNavfield() override { return true; };

    static POD_HanselChargeSkill pod;
    virtual void OnInterruption() override;
    static float GetMaxRange();
    static float GetDamage();

private:
    coroutine::Coroutine SpawningFieldEffect(std::weak_ptr<HanselChargeSkill> skill);
    std::weak_ptr<UnitAcquisitionSphereCollider> stompCollider;
    std::weak_ptr<ManagedFBX> stompEffect1;
    //std::weak_ptr<ManagedFBX> stompEffect2;
    std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;
    std::weak_ptr<yunutyEngine::graphics::Animator> animator;
    std::weak_ptr<VFXAnimator> stompEffect1Animator;
//    std::weak_ptr<VFXAnimator> stompEffect2Animator;


    float colliderEffectRatio;
    float foreswingSpeed = 1.f;
    float skillSpeed = 1.f;
    float jumpSpeed = 1.f;
    float backswingSpeed = 1.f;
};






template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::HANSEL_Q>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_HanselChargeSkill>>(HanselChargeSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::HANSEL_Q>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_HanselChargeSkill>>(HanselChargeSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::HANSEL_Q>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_HanselChargeSkill>>(HanselChargeSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::HANSEL_Q>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_HanselChargeSkill>>(HanselChargeSkill::pod, data["POD"]);
    return true;
}
