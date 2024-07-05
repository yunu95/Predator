#pragma once
#include "Skill.h"

class UnitAcquisitionSphereCollider;
class ManagedFBX;
struct POD_EnemyImpaleSkill
{
    float foreswingDuration = -1.0f;
    float backswingDuration = -1.0f;

    float impaleSkillDamage = 20.0f;
    float impaleSkillCoolTime = 6.0f;
    float impaleSkillApproachingTime = 2.0f;
    float impaleSkillRange = 8.0f;
    float impaleSkillWidth = 2.0f;
    float impaleSkillFirstSpearOffset = 0.5f;
    float impaleSkillMaxHeightPerSpear = 2.5f;
    float impaleSkillMinHeightPerSpear = 0.0f;
    float impaleSkillDurationPerSpear = 0.5f;
    float impaleSkillAriseDistancePerSpear = 0.5f;
    float impaleSkillAriseTimeNoisePerSpear = 0.5f;
    float impaleSkillAriseDistanceNoisePerSpear = 0.5f;
    float impaleSkillDuration = 2.0f;
    float impaleSkillKnockbackDistance = 2.0f;
    float impaleSkillKnockbackDuration = 2.0f;

    float impaleStartDelay = 1.5f;

    TO_JSON(POD_EnemyImpaleSkill)
        FROM_JSON(POD_EnemyImpaleSkill)
};

class EnemyImpaleSkill : public Skill
{
public:
    EnemyImpaleSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::EnemyImpale; }
    virtual float GetCastRange() override { return pod.impaleSkillRange; };
    virtual coroutine::Coroutine operator()() override;
    virtual void OnInterruption() override;

    virtual void OnPause() override;
    virtual void OnResume() override;

    static POD_EnemyImpaleSkill pod;

private:
    coroutine::Coroutine SpearArise(std::weak_ptr<EnemyImpaleSkill> skill, Vector2d pos);
    coroutine::Coroutine SpawningSkillffect(std::weak_ptr<EnemyImpaleSkill> skill);

    std::weak_ptr<coroutine::Coroutine> effectCoroutine;
    std::weak_ptr<ManagedFBX> previewEffect;
    std::weak_ptr<ManagedFBX> impaleEffect;
    std::weak_ptr<VFXAnimator> previewEffectAnimator;
    std::weak_ptr<VFXAnimator> impaleEffectAnimator;
    std::weak_ptr<yunutyEngine::graphics::Animator> animator;

    static std::queue<std::weak_ptr<UnitAcquisitionSphereCollider>> knockbackColliderQueue;
    static std::queue<std::weak_ptr<ManagedFBX>> spearFbxQueue;
    std::unordered_set<Unit*> damagedUnits;
    
    float foreswingSpeed = 1.f;
    float backswingSpeed = 1.f;
};

template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::EnemyImpale>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_EnemyImpaleSkill>>(EnemyImpaleSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::EnemyImpale>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_EnemyImpaleSkill>>(EnemyImpaleSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::EnemyImpale>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_EnemyImpaleSkill>>(EnemyImpaleSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::EnemyImpale>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_EnemyImpaleSkill>>(EnemyImpaleSkill::pod, data["POD"]);
    return true;
}
