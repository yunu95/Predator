#pragma once
#include "Skill.h"

struct POD_RobinChargeSkill
{
    float coolTime = 2.0f;
    float cost = 10.0f;
    float maxDistance = 8.0f;
    float rushSpeed = 20.0f;
    float damageRush = 20.0f;
    float damageImpact = 80.0f;
    float rushKnockbackDistance = 3.0f;
    float impactKnockbackDistance = 8.0f;
    float rushKnockbackDuration = 0.5f;
    float impactKnockbackDuration = 1.0f;
    float impactStunDuration = 2.0f;
    float rushKnockbackRadius = 1.0f;
    float impactKnockbackRadius = 5.0f;

    TO_JSON(POD_RobinChargeSkill)
    FROM_JSON(POD_RobinChargeSkill)
};

class UnitAcquisitionSphereCollider;
class RobinChargeSkill : public Skill
{
public:
    RobinChargeSkill() {};
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::ROBIN_Q; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;

    static POD_RobinChargeSkill pod;

private:
    std::weak_ptr<UnitAcquisitionSphereCollider> knockbackCollider;
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
