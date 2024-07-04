#include "RangedKitingController.h"
#include "InWanderLand.h"

void RangedKitingController::RegisterUnit(std::weak_ptr<Unit> unit)
{
    EnemyController::RegisterUnit(unit);
}

coroutine::Coroutine RangedKitingController::RoutinePerUnit(std::weak_ptr<Unit> unit)
{
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    auto unitPtr = unit.lock();
    // 공격할 수 없는 상황이면 도주
    if (unitPtr->referenceBlockAttack.BeingReferenced())
    {
        if (auto closestEnemy = unitPtr->GetClosestEnemy().lock())
        {
            auto distanceFromThreat = Unit::Distance(unitPtr, closestEnemy);
            if (distanceFromThreat < gc.enemyKitingDistanceThreshold)
            {
                Vector3d unitPos = unitPtr->GetTransform()->GetWorldPosition();
                Vector3d optimalPos{};
                for (float deltaAngle = math::Random::GetRandomFloat(0, 90); deltaAngle < 360; deltaAngle += 90)
                {
                    Vector3d deltaDistance = (-distanceFromThreat + unitPtr->GetUnitTemplateData().pod.m_atkRadius) * Quaternion{ Vector3d{0, deltaAngle, 0} }.Forward();
                    Vector3d destination = SingleNavigationField::Instance().GetClosestPointOnField(unitPos + deltaDistance);
                    auto screenPos{ graphics::Camera::GetMainCamera()->GetGI().GetScreenPos(destination) };
                    if (screenPos.x < -1 || screenPos.x>1 || screenPos.y < -1 || screenPos.y>1)
                    {
                        continue;
                    }

                    if (Unit::Distance(closestEnemy.get(), destination) > Unit::Distance(closestEnemy.get(), optimalPos))
                    {
                        optimalPos = destination;
                    }
                }
                unitPtr->OrderMove(optimalPos);
            }
        }
    }
    else
    {
        unitPtr->OrderAttackMove();
    }
    co_yield coroutine::WaitForSeconds{ gc.enemyKitingCooltime + math::Random::GetRandomFloat(0.1f) };
    co_return;
}