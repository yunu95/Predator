#pragma once
#include "Skill.h"

struct POD_HanselChargeSkill
{
    float	coolTime = 2.0f;
    float	skillCost = 10.0f;
    float	maxRange = 10.0f;
    float	maxJumpHeight = 5.0f;
    float   damage = 10.0f;
    float   stompRadius = 10.0f;

    TO_JSON(POD_HanselChargeSkill)
    FROM_JSON(POD_HanselChargeSkill)
};

class HanselChargeSkill : public Skill
{
public:
    HanselChargeSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::HANSEL_Q; }
    virtual float GetCastRange() override;
    virtual coroutine::Coroutine operator()()override;

    static POD_HanselChargeSkill pod;
    virtual void OnInterruption()override;
private:
    coroutine::Coroutine SpawningFieldEffect(std::weak_ptr<HanselChargeSkill> skill);
    std::weak_ptr<UnitAcquisitionSphereCollider> stompCollider;
    std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;
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
