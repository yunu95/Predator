#pragma once
#include "Skill.h"

struct POD_UrsulaParalysisSkill
{

    TO_JSON(POD_UrsulaParalysisSkill)
        FROM_JSON(POD_UrsulaParalysisSkill)
};

class UrsulaParalysisSkill : public Skill
{
public:
    Vector3d targetPos;
    UrsulaParalysisSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::URSULA_W; }
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
