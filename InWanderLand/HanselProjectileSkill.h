#pragma once
#include "Skill.h"

struct POD_HanselProjectileSkill
{

    TO_JSON(POD_HanselProjectileSkill)
        FROM_JSON(POD_HanselProjectileSkill)
};

class HanselProjectileSkill : public Skill
{
public:
    Vector3d targetPos;
    HanselProjectileSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::HANSEL_W; }
    virtual coroutine::Coroutine operator()()override;

    static POD_HanselProjectileSkill pod;
};






template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::HANSEL_W>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_HanselProjectileSkill>>(HanselProjectileSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::HANSEL_W>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_HanselProjectileSkill>>(HanselProjectileSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::HANSEL_W>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_HanselProjectileSkill>>(HanselProjectileSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::HANSEL_W>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_HanselProjectileSkill>>(HanselProjectileSkill::pod, data["POD"]);
    return true;
}
