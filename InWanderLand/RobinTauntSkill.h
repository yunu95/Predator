#pragma once
#include "Skill.h"

struct POD_RobinTauntSkill
{
    float   foreswingDuration = -1.0f;
    float	skillDuration = -1.0f;
    
    float	coolTime = 2.0f;
    float	skillCost = 10.0f;
    float   skillRadius = 1.0f;
    float   skillRadiusUpgraded = 1.0f;
    float	skillDamage = 5.0f;
    float	skillDamageUpgraded = 5.0f;
    float	skillTauntTime = 3.0f;

    float   zoomFactor = 1.0f;
    TO_JSON(POD_RobinTauntSkill)
        FROM_JSON(POD_RobinTauntSkill)
};

class UnitAcquisitionSphereCollider;
class RobinTauntSkill : public Skill
{
public:
    RobinTauntSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::ROBIN_W; }
    virtual float GetCastRange() override { return GetSkillRadius(); }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;

    static float GetSkillRadius(); 
    static float GetSkillDamage(); 
    static POD_RobinTauntSkill pod;

private:
    coroutine::Coroutine SpawningSkillffect(std::weak_ptr<RobinTauntSkill> skill);
    std::weak_ptr<coroutine::Coroutine> effectColliderCoroutine;
    std::weak_ptr<UnitAcquisitionSphereCollider> tauntCollider;
    std::weak_ptr<ManagedFBX> tauntEffect;
    std::weak_ptr<VFXAnimator> chargeEffectAnimator;
    std::weak_ptr<yunutyEngine::graphics::Animator> animator;
    yunuGI::IAnimation* tauntAnim = nullptr;

    float colliderEffectRatio;
    float foreswingSpeed = 1.f;
    float skillSpeed = 1.f;

    float localForeswingDuration = 0;
    float localBackswingDuration = 0;
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
