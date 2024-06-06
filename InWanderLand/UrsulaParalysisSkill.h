#pragma once
#include "Skill.h"

struct POD_UrsulaParalysisSkill
{
    float skillCoolTime = 2.0f;
    float skillCost = 10.0f;
    float skillRange = 6.0f;
    float skillRadius = 3.f;
    float skillDamageInterval = 0.3f;
    float skillDamagePerTick = 1.0f;
    float skillStunDuration = 1.0f;
    float skillPullingDuration = 0.5f;

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

    static POD_UrsulaParalysisSkill pod;
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
