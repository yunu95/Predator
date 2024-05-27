#include "CursorDetector.h"
#include "Unit.h"
#include "DebugMeshes.h"
#include "UnitProductor.h"

void CursorDetector::OnUnitEnter(std::weak_ptr<Unit> hostile)
{
}
void CursorDetector::OnUnitExit(std::weak_ptr<Unit> hostile)
{
}
void CursorDetector::OnHostileEnter(std::weak_ptr<Unit> hostile)
{
}
void CursorDetector::OnUnitEnter(std::weak_ptr<Unit> hostile)
{
}
void CursorDetector::OnUnitExit(std::weak_ptr<Unit> hostile)
{
}
void CursorDetector::OnAllyEnter(std::weak_ptr<Unit> hostile)
{

}
void CursorDetector::OnHostileExit(std::weak_ptr<Unit> hostile)
{
    DetermineClosestEnemy();
}
void CursorDetector::OnAllyExit(std::weak_ptr<Unit> hostile)
{

}

void CursorDetector::Start()
{
    m_SelectEffectObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    AttachDebugMesh(m_SelectEffectObject, DebugMeshType::Sphere, yunuGI::Color::red(), true);
    m_SelectEffectObject->GetTransform()->SetWorldScale({ UNIT_LENGTH * 1.5f, 0.3f, UNIT_LENGTH * 1.5f });
    m_SelectEffectObject->SetSelfActive(false);
}

void CursorDetector::Update()
{
    UnitAcquisitionSphereCollider::Update();

    if (!m_currentSelectedUnit.expired())
    {
        m_SelectEffectObject->GetTransform()->SetWorldPosition(m_currentSelectedUnit.lock()->GetTransform()->GetWorldPosition() + Vector3d::up * 0.1f);
    }
}
std::weak_ptr<Unit> CursorDetector::GetCurrentOnMouseUnit() const
{
    return m_currentSelectedUnit;
}

void CursorDetector::DetermineClosestUnit()
{
    float shortestDistance;
    bool isCalculateStarted{ false };
    if (!foes.empty())
    {
        for (auto e : foes)
        {
            float nowComparingDistance = (GetTransform()->GetWorldPosition() - e->GetTransform()->GetWorldPosition()).Magnitude();
            if (!isCalculateStarted)
            {
                shortestDistance = nowComparingDistance;
                m_currentSelectedUnit = e;
                isCalculateStarted = true;
            }
            else if (shortestDistance > nowComparingDistance)
            {
                shortestDistance = nowComparingDistance;
                m_currentSelectedUnit = e;
            }
        }
        /// Select 관련 오브젝트를 m_currentSelectedUnit 위치에 띄우는 로직
        m_SelectEffectObject->GetTransform()->SetWorldPosition(m_currentSelectedUnit.lock()->GetTransform()->GetWorldPosition() + Vector3d(0, 0.1f, 0));
        m_SelectEffectObject->SetSelfActive(true);
    }
    else
    {
        m_currentSelectedUnit.reset();
        /// Select 관련 오브젝트를 비활성화 하는 로직
        m_SelectEffectObject->SetSelfActive(false);
    }

}
