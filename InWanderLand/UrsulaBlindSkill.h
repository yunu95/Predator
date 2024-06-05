#pragma once
#include "Skill.h"

struct POD_UrsulaBlindSkill
{
    float ursulaQSkillCoolTime = 2.0f;
    float ursulaQSkillCost = 10.0f;
    float ursulaQSkillRadius = 3.f;
    float ursulaQSkillOffset = 3.f;
    float ursulaQSkillProjectileSpeed = 15.f;
    float ursulaQSkillBlindDuration = 3.0f;
    float ursulaQSkillFieldDuration = 3.0f;
    float ursulaQSkillFieldDamagePerTick = 1.0f;
    float ursulaQSkillFieldDamageInterval = 0.3f;

    TO_JSON(POD_UrsulaBlindSkill)
        FROM_JSON(POD_UrsulaBlindSkill)
};

class UrsulaBlindSkill : public Skill
{
public:
    Vector3d targetPos;
    UrsulaBlindSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::URSULA_Q; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;

    static POD_UrsulaBlindSkill pod;

    /// 삼각형 대형으로 보았을 때, Top / Left / Right 위치값을 반환합니다.
    static Vector3d GetSkillObjectPos_Top();
    static Vector3d GetSkillObjectPos_Left();
    static Vector3d GetSkillObjectPos_Right();
private:
    static Vector3d lastSkillPos;
    static Vector3d lastSkillDir;
    std::weak_ptr<UnitAcquisitionSphereCollider> projectile;
    std::weak_ptr<UnitAcquisitionSphereCollider> circle_Top;
    std::weak_ptr<UnitAcquisitionSphereCollider> circle_Left;
    std::weak_ptr<UnitAcquisitionSphereCollider> circle_Right;
};






template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::URSULA_Q>(json& data)
{
    application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_UrsulaBlindSkill>>(UrsulaBlindSkill::pod, data["POD"]);
    return true;
}


template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::URSULA_Q>(json& data)
{
    application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_UrsulaBlindSkill>>(UrsulaBlindSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::URSULA_Q>(const json& data)
{
    application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_UrsulaBlindSkill>>(UrsulaBlindSkill::pod, data["POD"]);
    return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::URSULA_Q>(const json& data)
{
    application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_UrsulaBlindSkill>>(UrsulaBlindSkill::pod, data["POD"]);
    return true;
}
