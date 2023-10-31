#include "Unit.h"
#include "Dotween.h"
#include "ProjectileSystem.h"
#include "Projectile.h"

void Unit::Start()
{
	unitType = "Enemy";
	m_speed = 5.0f;
	m_bulletSpeed = 1.0f;
	isInIDRange = false;
	isInAtkRange = false;
	isOutIDRange = false;
	isOutAtkRange = false;
	
	unitFSM = new FSM<UnitState>(UnitState::Idle);

	unitFSM->transitions[UnitState::Idle].push_back({ UnitState::Chase,
		[this]() { return isInIDRange == true; } });

	unitFSM->transitions[UnitState::Chase].push_back({ UnitState::Attack,
		[this]() { return isInAtkRange == true; } });

	unitFSM->transitions[UnitState::Chase].push_back({ UnitState::Idle,
	[this]() { return isOutIDRange == true; } });

	unitFSM->transitions[UnitState::Attack].push_back({ UnitState::Chase,
	[this]() { return isOutAtkRange == true; } });

	unitFSM->engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
	unitFSM->engageAction[UnitState::Chase] = [this]() { ChaseEngage(); };
	unitFSM->engageAction[UnitState::Attack] = [this]() { AttackEngage(); };


	unitFSM->updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
	unitFSM->updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };

}

void Unit::Update()
{
	unitFSM->UpdateState();

	// 인식 범위 내에 들어오게 된다면, 목표로 하는(적군) 오브젝트의 위치 정보를 계속 받아와야한다.
	// 그렇게 되면 이동중에 해당 적군 오브젝트의 위치가 바뀌어도 그에 맞게 자연스럽게 이동할 수 있지 않을까?
}

void Unit::IdentifyTransition()
{
	isInIDRange = true;			// FSM Transition Activated
}

void Unit::ExitIDRangeTransition()
{
	isOutIDRange = true;
}

void Unit::SetOpponentGameObject(GameObject* obj)
{
	m_opponentGameobject = obj;
}

void Unit::AttackTransition()
{
	isInAtkRange = true;
}

void Unit::IdleFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });

	GetGameObject()->GetComponent<Dotween>()->StopAllDotweenFunction();
}

void Unit::AttackFunction()
{
	// 상대방을 공격하는 로직. 근접 공격 or 원거리 (투사체) 공격
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });

	// 투사체가 설정되어 있다면 원거리 공격을 한다.
	ProjectileSystem::GetInstance()->Shoot(this, m_opponentGameobject->GetTransform()->GetWorldPosition(), m_bulletSpeed);

	/// 공격 후 m_Delay간격으로 재귀를 할지, 아니면 쫓아갈 지를 정한다.
	GetGameObject()->GetComponent<Dotween>()->DONothing(1.5f/*딜레이*/).OnComplete([this]()
		{
			float distance = (m_opponentGameobject->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
			if (distance > m_AtkDistance)
			{
				isOutAtkRange = true;
			}
			else
			{
				AttackFunction();
			}
		});
}

void Unit::ChaseFunction()
{
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });

	float distance = (m_opponentGameobject->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetTransform()->GetWorldPosition()).Magnitude();

	Vector3d betweenVector = m_opponentGameobject->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition();
	Vector3d directionVector = betweenVector.Normalized();

	Vector3d endPosition = GetGameObject()->GetTransform()->GetWorldPosition() + directionVector * (distance - m_AtkDistance);

	GetGameObject()->GetComponent<Dotween>()->DOMove(endPosition, distance / m_speed).OnComplete([=]()
		{
			float distance = (m_opponentGameobject->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
			if (distance > m_AtkDistance)
			{
				ChaseFunction();
			}
			else
			{
				AttackTransition();
			}
		});
}

void Unit::IdleEngage()
{
	isOutIDRange = false;
	IdleFunction();
}

void Unit::ChaseEngage()
{
	isInIDRange = false;	// 상태가 바뀌면 해당 멤버 bool 변수를 다시 초기화 해준다.
	isOutAtkRange = false;
	ChaseFunction();
}

void Unit::AttackEngage()
{
	isInAtkRange = false;
	AttackFunction();
}

void Unit::ChaseUpdate()
{
}

void Unit::AttackUpdate()
{
}

std::string Unit::GetType() const
{
	return unitType;
}

void Unit::SetIdRadius(float radius)
{
	m_IdDistance = radius;
}

void Unit::SetAtkRadius(float radius)
{
	m_AtkDistance = radius;
}
