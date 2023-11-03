#include "Unit.h"
#include "Dotween.h"
#include "ProjectileSystem.h"
#include "Projectile.h"

void Unit::Start()
{
	unitType = "Enemy";
	m_speed = 3.0f;
	m_bulletSpeed = 5.1f;

	transitionTriggerVector.push_back(idleToChase);
	transitionTriggerVector.push_back(idleToAttack);
	transitionTriggerVector.push_back(chaseToIdle);
	transitionTriggerVector.push_back(attackToIdle);
	InitTriggers();

	unitFSM = new FSM<UnitState>(UnitState::Idle);
	 
	unitFSM->transitions[UnitState::Idle].push_back({ UnitState::Chase,
		[this]() { return idleToChase == true; } });

	unitFSM->transitions[UnitState::Idle].push_back({ UnitState::Attack,
		[this]() { return idleToAttack == true; } });

	unitFSM->transitions[UnitState::Chase].push_back({ UnitState::Idle,
		[this]() { return chaseToIdle == true; } });

	unitFSM->transitions[UnitState::Attack].push_back({ UnitState::Idle,
		[this]() { return attackToIdle == true; } });

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
	if (m_opponentGameobject != nullptr)
		m_opponentPosition = m_opponentGameobject->GetTransform()->GetWorldPosition();
}

#pragma region TransitionFuctions
	void Unit::IdleTransition()
	{
		// Chase로 갈 것인지, Attack으로 갈 것인지 판별
		// -> idleToChase냐 idleToAttack 이냐!
		float distance = (m_opponentPosition - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

		if (distance < m_IdDistance && distance > m_AtkDistance)
			idleToChase = true;
		
		if (distance <= m_AtkDistance)
			idleToAttack = true;
		
	}

	void Unit::ChaseTransition()
	{
		chaseToIdle = true;
	}

	void Unit::AttackTransition()
	{
		attackToIdle = true;
	}
#pragma endregion

void Unit::SetOpponentGameObject(GameObject* obj)
{
	m_opponentGameobject = obj;
}

void Unit::EnterIDRange()
{
	isJustEntered = true;
}

void Unit::ExitIDRange()
{
	isJustEntered = false;
}

#pragma region State Engage Functions
	void Unit::IdleEngageFunction()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });

		GetGameObject()->GetComponent<Dotween>()->StopAllDotweenFunction();
	
		IdleTransition();
	}

	void Unit::AttackEngageFunction()
	{
		// 상대방을 공격하는 로직. 근접 공격 or 원거리 (투사체) 공격
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });

		// 투사체가 설정되어 있다면 원거리 공격을 한다.
		ProjectileSystem::GetInstance()->Shoot(this, m_opponentGameobject->GetComponent<Unit>(), m_bulletSpeed);

		/// 공격 후 m_Delay간격으로 재귀를 할지, 아니면 쫓아갈 지를 정한다.
		GetGameObject()->GetComponent<Dotween>()->DONothing(0.5f/*딜레이*/).OnComplete([this]()
			{
				float distance = (m_opponentGameobject->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

				if (distance > m_AtkDistance)
				{
					AttackEngageFunction();
				}
				else
				{
					AttackTransition();
				}
			});
	}
#pragma endregion 

#pragma region State Update Functions
	void Unit::ChaseUpdateFunction()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });

		Vector3d betweenVector = m_opponentGameobject->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition();
		Vector3d directionVector = betweenVector.Normalized();

		Vector3d endPosition = GetGameObject()->GetTransform()->GetWorldPosition() + (directionVector * m_speed * Time::GetDeltaTime());

		GetGameObject()->GetComponent<NavigationAgent>()->SetSpeed(1.0f);
		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_opponentPosition);

		float distance = (m_opponentPosition - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

		if (distance < m_AtkDistance)
		{
			ChaseTransition();
		}
		//else if (distance > m_IdDistance)
		//{
		//	ExitIDRangeTransition();
		//}
	}
#pragma endregion 

void Unit::IdleEngage()
{
	InitTriggers();
	IdleEngageFunction();
}

void Unit::ChaseEngage()
{
	InitTriggers();
}

void Unit::AttackEngage()
{
	InitTriggers();

	AttackEngageFunction();
}

void Unit::ChaseUpdate()
{
	ChaseUpdateFunction();
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

void Unit::InitTriggers()
{
	for (auto e : transitionTriggerVector)
	{
		e = false;
	}
}
