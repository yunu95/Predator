#pragma once
#include "Skill.h"

struct POD_RobinTauntSkill
{
    float	robinESkillCoolTime = 2.0f;
    float	robinESkillCost = 10.0f;
    float	robinESkillRadius = 6.0f;
    float	robinESkillDamage = 5.0f;
    float	robinESkillPlayTime = 2.0f;
    float	robinESkillTauntTime = 3.0f;

    TO_JSON(POD_RobinTauntSkill)
    FROM_JSON(POD_RobinTauntSkill)
};

class UnitAcquisitionSphereCollider;
class RobinTauntSkill : public Skill
{
public:
    Vector3d targetPos;
    RobinTauntSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::ROBIN_W; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;

    static POD_RobinTauntSkill pod;

private:
    std::weak_ptr<UnitAcquisitionSphereCollider> tauntCollider;
};






template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::ROBIN_W>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_RobinTauntSkill>>(RobinTauntSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::ROBIN_W>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_RobinTauntSkill>>(RobinTauntSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::ROBIN_W>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_RobinTauntSkill>>(RobinTauntSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::ROBIN_W>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_RobinTauntSkill>>(RobinTauntSkill::pod, data["POD"]);
    return true;
}
