#pragma once
#include "Skill.h"

struct POD_UrsulaBlindSkill
{
    float skillCoolTime = 2.0f;
    float skillCost = 10.0f;
    float skillRange = 6.f;
    //float skillRadius = 3.f;
    float skillScale = 1.0f;
    float skillOffset = 1.f;
    float skillDamage = 5.f;
    float skillBlindTime = 3.0f;
    

    TO_JSON(POD_UrsulaBlindSkill)
        FROM_JSON(POD_UrsulaBlindSkill)
};

class UrsulaBlindSkill : public Skill
{
private:
	coroutine::Coroutine SpawningFieldEffect(std::weak_ptr<UrsulaBlindSkill> skill);

	static Vector3d skillStart;
	static Vector3d skillDestination;
	std::weak_ptr<UnitAcquisitionSphereCollider> circle_Top;
	std::weak_ptr<UnitAcquisitionSphereCollider> circle_Left;
	std::weak_ptr<UnitAcquisitionSphereCollider> circle_Right;

	std::weak_ptr<ManagedFBX> onUrsulaPosEffect;
	std::weak_ptr<ManagedFBX> onTargetPosEffect1;
	std::weak_ptr<ManagedFBX> onTargetPosEffect2;
	std::weak_ptr<ManagedFBX> onTargetPosEffect3;

public:
    UrsulaBlindSkill() {}
    virtual SkillType::Enum GetSkillType() { return SkillType::Enum::URSULA_Q; }
    virtual float GetCastRange() override { return pod.skillRange; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;

    static POD_UrsulaBlindSkill pod;

    /// 스킬 가이드를 위해서 출발지와 도착지를 세팅하는 부분입니다.
    /// 스킬을 시전할 때에는 owner Pos 와 targetPos 를 통해 자동으로 갱신합니다.
    static void UpdatePosition(const Vector3d& start, const Vector3d& dest);

    /// 삼각형 대형으로 보았을 때, Top / Left / Right 위치값을 반환합니다.
    static Vector3d GetSkillObjectPos_Top(const Vector3d& dest);
    static Vector3d GetSkillObjectPos_Left(const Vector3d& dest);
    static Vector3d GetSkillObjectPos_Right(const Vector3d& dest);

    static float colliderEffectRatio;
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
