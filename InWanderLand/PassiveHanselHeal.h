#pragma once
#include "PassiveSkill.h"

struct POD_PassiveHanselHeal
{
    float cookieLifetime = 10.0f;
    float healAmount = 10.0f;
    int hitsRequired = 5;
    int cookieBatchSize = 5;
    float cookieScale = 1.0f;
    float cookieRadius = 1.0f;

    TO_JSON(POD_PassiveHanselHeal)
        FROM_JSON(POD_PassiveHanselHeal)
};

class PassiveHanselHeal : public PassiveSkill
{
public:
    virtual void Init(std::weak_ptr<Unit> owner) override;
    virtual SkillType::Enum GetSkillType() { return SkillType::Passive_Hansel_Heal; };
    static POD_PassiveHanselHeal pod;
private:
    int hitCounter = 0;
    void IncrementHitCounter();
    coroutine::Coroutine CookieLingering(const Vector3d& pos, std::weak_ptr<Unit> owner);
    coroutine::Coroutine CookieDisappear(const Vector3d& pos, std::weak_ptr<Unit> owner);
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