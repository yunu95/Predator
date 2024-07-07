#pragma once
#include "Skill.h"

struct POD_HanselProjectileSkill
{
    float foreswingDuration = -1.0f;
    float backswingDuration = -1.0f;

    float projectileSpeed = 10.0f;
    float skillRadius = 1.0f;
    float maxRange = 10.0f;
    float maxRangeUpgraded = 10.0f;
    float coolTime = 2.0f;
    float skillCost = 10.0f;
    float pieRotateSpeed = 1080.0f;
    float pieOffsetY = 2.0f;
    float pieOffsetX = 0.0f;
    float healPoint = 5.0f;
    float buffDuration = 10.0f;
    float debuffDuration = 10.0f;
    float critChanceBonus = 0.1f;
    float attackSpeedBonus = 0.5f;
    float debuffAttackSpeedAdder = -0.5f;
    float debuffAttackDamageMultiplier = 0.5f;
    int hitCount = 1;
    int hitCountUpgraded = 1;

    float   zoomFactor = 1.0f;

    TO_JSON(POD_HanselProjectileSkill)
        FROM_JSON(POD_HanselProjectileSkill)
};

class HanselProjectileSkill : public Skill
{
private:
    coroutine::Coroutine ThrowingPie(std::weak_ptr<HanselProjectileSkill> skill);
    //coroutine::Coroutine SpawningSkillffect(std::weak_ptr<Unit> unit);

    std::weak_ptr<UnitAcquisitionSphereCollider> pieCollider;
    std::weak_ptr<ManagedFBX> pieObject;
    std::weak_ptr<yunutyEngine::graphics::Animator> animator;
    yunuGI::IAnimation* throwAnim = nullptr;

    float colliderEffectRatio;
    float foreswingSpeed = 1.f;
    float backswingSpeed = 1.f;

public:
    HanselProjectileSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::HANSEL_W; }
    virtual float GetCastRange() override;
    virtual coroutine::Coroutine operator()() override;
    virtual void OnInterruption() override;
    static float GetMaxRange();
    static int GetHitCount();

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
