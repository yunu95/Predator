#pragma once
#include "PassiveSkill.h"

struct POD_PassiveRobinBleed
{
    float duration = 10.0f;
    float durationUpgraded = 10.0f;
    float dpsPerStack = 10.0f;
    int maxStack = 10;

    TO_JSON(POD_PassiveRobinBleed)
        FROM_JSON(POD_PassiveRobinBleed)
};

class PassiveRobinBleed : public PassiveSkill
{
public:
    virtual void Init(std::weak_ptr<Unit> owner) override;
    virtual SkillType::Enum GetSkillType() override { return SkillType::Passive_Robin_Bleeding; };
    static float BleedDuration(); 
    static POD_PassiveRobinBleed pod;
private:
};

template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::Passive_Robin_Bleeding>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_PassiveRobinBleed>>(PassiveRobinBleed::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::Passive_Robin_Bleeding>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_PassiveRobinBleed>>(PassiveRobinBleed::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::Passive_Robin_Bleeding>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_PassiveRobinBleed>>(PassiveRobinBleed::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::Passive_Robin_Bleeding>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_PassiveRobinBleed>>(PassiveRobinBleed::pod, data["POD"]);
    return true;
}