#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"

#include <math.h>

Vector3d UrsulaBlindSkill::lastSkillPos = Vector3d();
Vector3d UrsulaBlindSkill::lastSkillDir = Vector3d();

POD_UrsulaBlindSkill UrsulaBlindSkill::pod = POD_UrsulaBlindSkill();

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
    lastSkillPos = targetPos;
    lastSkillDir = (lastSkillPos - owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition()).Normalized();
    co_await std::suspend_always{};

    /// 투사체 던짐
    

    co_return;
}

void UrsulaBlindSkill::OnInterruption()
{
    projectile.lock()->SetRadius(0.5);
    circle_Top.lock()->SetRadius(0.5);
    circle_Left.lock()->SetRadius(0.5);
    circle_Right.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(projectile);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Top);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Left);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Right);
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Top()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    return lastSkillPos + std::sqrt(3) / 3 * lastSkillDir * length;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Left()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto left = Vector3d::Cross(lastSkillDir, Vector3d::up).Normalized();
    return lastSkillPos - std::sqrt(3) / 6 * lastSkillDir * length + left * length / 2;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Right()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto right = Vector3d::Cross(-lastSkillDir, Vector3d::up).Normalized();
    return lastSkillPos - std::sqrt(3) / 6 * lastSkillDir * length + right * length / 2;
}
