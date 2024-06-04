#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"

Vector3d UrsulaBlindSkill::lastSkillPos = Vector3d();
Vector3d UrsulaBlindSkill::lastSkillDir = Vector3d();

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
    lastSkillPos = targetPos;
    lastSkillDir = (lastSkillPos - owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition()).Normalized();
    co_await std::suspend_always{};

    

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
    auto finalPos = lastSkillPos;
    return finalPos;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Left()
{
    return Vector3d();
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Right()
{
    return Vector3d();
}
