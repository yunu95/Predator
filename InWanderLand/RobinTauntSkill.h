#pragma once
#include "Skill.h"

struct POD_RobinTauntSkill
{
    float	coolTime = 2.0f;
    float	skillCost = 10.0f;
    float   skillRadius = 1.0f;
    float	skillDamage = 5.0f;
    float	skillPlayTime = 2.0f;
    float	skillTauntTime = 3.0f;

    TO_JSON(POD_RobinTauntSkill)
    FROM_JSON(POD_RobinTauntSkill)
};

class UnitAcquisitionSphereCollider;
class RobinTauntSkill : public Skill
{
public:
    RobinTauntSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::ROBIN_W; }
    virtual float GetCastRange() override { return pod.skillRadius; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;

    static POD_RobinTauntSkill pod;

private:
    float colliderEffectRatio;
    coroutine::Coroutine SpawningSkillffect(std::weak_ptr<RobinTauntSkill> skill);
    std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;
    std::weak_ptr<UnitAcquisitionSphereCollider> tauntCollider;
    std::weak_ptr<ManagedFBX> tauntEffect;
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
