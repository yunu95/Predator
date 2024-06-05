#pragma once
#include "Skill.h"

struct POD_HanselChargeSkill
{
    

    TO_JSON(POD_HanselChargeSkill)
    FROM_JSON(POD_HanselChargeSkill)
};

class HanselChargeSkill : public Skill
{
public:
    Vector3d targetPos;
    HanselChargeSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::HANSEL_Q; }
    virtual coroutine::Coroutine operator()()override;

    static POD_HanselChargeSkill pod;
    virtual void OnInterruption()override;
private:
    std::weak_ptr<UnitAcquisitionSphereCollider> stompCollider;

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
