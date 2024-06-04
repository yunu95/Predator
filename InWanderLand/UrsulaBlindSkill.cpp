#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
    co_return;
}

void UrsulaBlindSkill::OnInterruption()
{
    projectile.lock()->SetRadius(0.5);
    circle_1.lock()->SetRadius(0.5);
    circle_2.lock()->SetRadius(0.5);
    circle_3.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(projectile);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_1);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_2);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_3);
}
