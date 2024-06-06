#pragma once
#include "Skill.h"

struct POD_RobinChargeSkill
{

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
