#pragma once
#include "Skill.h"

class UrsulaBlindSkill : public Skill
{
public:
    Vector3d targetPos;
    UrsulaBlindSkill(Vector3d targetPos) : targetPos(targetPos) {}
    virtual SkillType::Enum GetSkillType() { return SkillType::URSULA_Q; }
    virtual coroutine::Coroutine operator()()override;
    virtual void OnInterruption()override;

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

