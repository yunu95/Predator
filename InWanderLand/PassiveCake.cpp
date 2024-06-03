#include "PassiveCake.h"
#include "PassiveCakePool.h"
#include "Dotween.h"
#include "Unit.h"
#include "DebuggingMeshPool.h"

void PassiveCake::PopCake(Vector3d initialPos)
{
	GetTransform()->SetWorldPosition(initialPos);
	m_mesh->GetTransform()->SetWorldPosition(initialPos);

	isFunctionCalled = true;

	/// 떨어질 위치 선정 (랜덤)
	std::srand(std::time(0));
	int dropDistance = std::rand() % (maxDropDistance - minDropDistance) + minDropDistance;

	int dropAngle = std::rand() % maxDegree;

	GetGameObject()->GetTransform()->GetWorldRotation() = Quaternion({ 0, static_cast<float>(dropAngle), 0 });

	Vector3d endPos = GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * dropDistance;

	//GetGameObject()->SetSelfActive(true);

	//m_collider->SetActive(false);
	m_mesh->SetActive(true);

	m_dotween->DOMove(endPos, 2.0f).OnComplete([=]()
		{
			isDropped = true;
			m_collider->SetActive(true);
		});

}

void PassiveCake::SetMesh(yunutyEngine::graphics::StaticMeshRenderer* p_mesh)
{
	m_mesh = p_mesh;
}

void PassiveCake::SetCollider(physics::BoxCollider* p_collider)
{
	m_collider = p_collider;
	//m_collider->SetActive(false);
}

void PassiveCake::SetDotweenComponent(Dotween* p_dotween)
{
	m_dotween = p_dotween;
}

void PassiveCake::Start()
{
}

void PassiveCake::Update()
{
	if (isDropped)
	{
		cakeElapsed += Time::GetDeltaTime();

		if (cakeElapsed >= cakePersistTime)
		{
			m_mesh->SetActive(false);
			PassiveCakePool::Instance().Return(this);
			cakeElapsed = 0.0f;
			isDropped = false;
			isFunctionCalled = false;
		}
	}

	if (isFunctionCalled)
	{
		m_mesh->GetTransform()->SetWorldPosition(GetGameObject()->GetTransform()->GetWorldPosition());
	}
}

void PassiveCake::OnTriggerEnter(physics::Collider* collider)
{
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->IsPlayerUnit())
	{
		m_mesh->SetActive(false);
		m_collider->SetActive(false);
		PassiveCakePool::Instance().Return(this);
		auto debuggingMeshComponent = DebuggingMeshPool::Instance().Borrow();
		debuggingMeshComponent->SetUnitObject(colliderUnitComponent);
		debuggingMeshComponent->PopMeshUP(yunuGI::Color::blue(), MaterialNum::Red);
		colliderUnitComponent->Heal(cakeHealingPoint);
	}
}
