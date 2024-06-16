#pragma once
#include "PassiveSkill.h"

struct POD_PassiveUrsulaSpeed
{
    float duration = 10.0f;
    float durationUpgraded = 10.0f;
    float attackSpeedBonusPerStack = 0.1f;
    float attackSpeedBonusPerStackUpgraded = 0.1f;
    float critChanceBonusPerStack = 0.1f;
    float critChanceBonusPerStackUpgraded = 0.1f;
    int maxStack = 10;

    TO_JSON(POD_PassiveUrsulaSpeed)
        FROM_JSON(POD_PassiveUrsulaSpeed)
};

class PassiveUrsula : public PassiveSkill
{
public:
    virtual void Init(std::weak_ptr<Unit> owner) override;
    virtual SkillType::Enum GetSkillType() override { return SkillType::Passive_Ursula_Speed; };
    static POD_PassiveUrsulaSpeed pod;
    static float GetDuration();
    static float GetAttackSpeedBonusPerStack();
    static float GetCritChanceBonusPerStack();
private:
};

template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::Passive_Ursula_Speed>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_PassiveUrsulaSpeed>>(PassiveUrsula::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::Passive_Ursula_Speed>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_PassiveUrsulaSpeed>>(PassiveUrsula::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::Passive_Ursula_Speed>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_PassiveUrsulaSpeed>>(PassiveUrsula::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::Passive_Ursula_Speed>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_PassiveUrsulaSpeed>>(PassiveUrsula::pod, data["POD"]);
    return true;
}