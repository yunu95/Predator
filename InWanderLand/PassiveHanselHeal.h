#pragma once
#include "PassiveSkill.h"
#include "ProgressTracker.h"

struct POD_PassiveHanselHeal
{
    float cookieLifetime = 10.0f;
    float healAmount = 10.0f;
    float healAmountUpgraded = 10.0f;
    int hitsRequired = 5;
    int cookieBatchSize = 5;
    float cookieScale = 1.0f;
    float cookieRadius = 1.0f;
    float dropRadius = 3.0f;
    float maxHeight = 3.0f;
    float cakeOnAirDuration = 1.5;
  
    TO_JSON(POD_PassiveHanselHeal)
        FROM_JSON(POD_PassiveHanselHeal)
};

class ManagedFBX;
class UnitAcquisitionSphereCollider;
class PassiveHanselHeal : public PassiveSkill, public application::ProgressTracker
{
public:
    virtual void Init(std::weak_ptr<Unit> owner) override;
    virtual SkillType::Enum GetSkillType() { return SkillType::Passive_Hansel_Heal; };
    static float GetHealAmount();
    static POD_PassiveHanselHeal pod;

    virtual void OnPause() override;
    virtual void OnResume() override;

    virtual void Recovery() override;

private:
    struct CustomCompManagedFBX
    {
        bool operator()(const std::weak_ptr<ManagedFBX>& lp, const std::weak_ptr<ManagedFBX>& rp) const
        {
            return lp.lock().get() > rp.lock().get();
        }
    };

    struct CustomCompUnitAcquisitionSphereCollider
    {
        bool operator()(const std::weak_ptr<UnitAcquisitionSphereCollider>& lp, const std::weak_ptr<UnitAcquisitionSphereCollider>& rp) const
        {
            return lp.lock().get() > rp.lock().get();
        }
    };

    int hitCounter{ 0 };
    void IncrementHitCounter();
    coroutine::Coroutine CookieLingering(Vector3d pos, std::weak_ptr<Unit> owner);
    bool isPaused = false;

    static std::set<std::weak_ptr<ManagedFBX>, CustomCompManagedFBX> cookieContainer;
    static std::set<std::weak_ptr<UnitAcquisitionSphereCollider>, CustomCompUnitAcquisitionSphereCollider> colliderContainer;
};

template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::Passive_Hansel_Heal>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_PassiveHanselHeal>>(PassiveHanselHeal::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::Passive_Hansel_Heal>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_PassiveHanselHeal>>(PassiveHanselHeal::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::Passive_Hansel_Heal>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_PassiveHanselHeal>>(PassiveHanselHeal::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::Passive_Hansel_Heal>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_PassiveHanselHeal>>(PassiveHanselHeal::pod, data["POD"]);
    return true;
}