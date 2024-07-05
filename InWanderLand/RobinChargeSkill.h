#pragma once
#include "Skill.h"

struct POD_RobinChargeSkill
{
    float foreswingDuration = -1;
    float backswingDuration = -1;
    float coolTime = 2.0f;
    float cost = 10.0f;
    float maxDistance = 8.0f;
    float maxDistanceUpgraded = 8.0f;
    float rushSpeed = 20.0f;
    float damageRush = 20.0f;
    float damageRushUpgraded = 20.0f;
    float damageImpact = 80.0f;
    float damageImpactUpgraded = 80.0f;
    float rushKnockbackDistance = 3.0f;
    float impactKnockbackDistance = 8.0f;
    float rushKnockbackDuration = 0.5f;
    float impactKnockbackDuration = 1.0f;
    float impactStunDuration = 2.0f;
    float rushKnockbackRadius = 1.0f;
    float impactKnockbackRadius = 5.0f;
    float effectScale = 1.0f;
    float impactCamShakeDistance = 0.5f;
    float impactCamShakeFrequency = 10.0f;
    float impactCamShakeDecreaseFactor = 0.4f;

    TO_JSON(POD_RobinChargeSkill)
        FROM_JSON(POD_RobinChargeSkill)
};

class UnitAcquisitionSphereCollider;
class RobinChargeSkill : public Skill
{
public:
    RobinChargeSkill() {};
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::ROBIN_Q; }
    virtual float GetCastRange() override { return GetMaxDistance(); };
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;
    //virtual bool ShouldTargetOnNavfield() override { return true; };

    static POD_RobinChargeSkill pod;
    static float GetMaxDistance();
    static float GetDamageRush();
    static float GetDamageImpact();

private:
    coroutine::Coroutine SpawningSkillffect(std::weak_ptr<RobinChargeSkill> skill, Vector3d skillStartPos);
    std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
    std::weak_ptr<ManagedFBX> chargeEffect;
    yunuGI::IAnimation* rushAnim = nullptr;
    yunuGI::IAnimation* slamAnim = nullptr;
    std::weak_ptr<VFXAnimator> chargeEffectAnimator = std::weak_ptr<VFXAnimator>();
    float foreswingSpeed = 1.f;
    float backswingSpeed = 1.f;
    float effectSpeed = 1.f;
};


template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::ROBIN_Q>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_RobinChargeSkill>>(RobinChargeSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::ROBIN_Q>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_RobinChargeSkill>>(RobinChargeSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::ROBIN_Q>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_RobinChargeSkill>>(RobinChargeSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::ROBIN_Q>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_RobinChargeSkill>>(RobinChargeSkill::pod, data["POD"]);
    return true;
}
