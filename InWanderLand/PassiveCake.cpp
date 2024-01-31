#include "PassiveCake.h"
#include "PassiveCakePool.h"
#include "Dotween.h"
#include "Unit.h"

void PassiveCake::PopCake(Vector3d initialPos)
{
    /// 떨어질 위치 선정 (랜덤)
    std::srand(std::time(0));
    int dropDistance = std::rand() % (maxDropDistance - minDropDistance) + minDropDistance;

    int dropAngle = std::rand() % maxDegree;

    GetTransform()->SetWorldPosition(initialPos);

    GetGameObject()->GetTransform()->SetLocalRotation(Quaternion({ 0, static_cast<float>(dropAngle), 0 }));

    Vector3d endPos = GetTransform()->GetWorldPosition() + GetTransform()->GetLocalRotation().Forward() * dropDistance;

    m_collider->SetActive(false);

    m_mesh->SetActive(true);

    GetGameObject()->GetComponent<Dotween>()->DOMove(endPos, 2.0f).OnComplete([this]()
        {
            isDropped = true;
            m_collider->SetActive(true);
        });
}

void PassiveCake::SetMesh(yunutyEngine::graphics::StaticMeshRenderer* p_mesh)
{
    m_mesh = p_mesh;
    m_mesh->SetActive(false);
}

void PassiveCake::SetCollider(physics::BoxCollider* p_collider)
{
    m_collider = p_collider;
    m_collider->SetActive(false);
}

void PassiveCake::Start()
{
    //GetGameObject()->SetSelfActive(false);

}

void PassiveCake::Update()
{
    if (isDropped)
    {
        cakeElapsed += Time::GetDeltaTime();

        if (cakeElapsed >= cakePersistTime)
        {
            PassiveCakePool::SingleInstance().Return(this);
            cakeElapsed = 0.0f;
            isDropped = false;
        }
    }
}

void PassiveCake::OnTriggerEnter(physics::Collider* collider)
{
    if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
    {
        m_mesh->SetActive(false);
        PassiveCakePool::SingleInstance().Return(this);
        colliderUnitComponent->Heal(cakeHealingPoint);
    }
}
