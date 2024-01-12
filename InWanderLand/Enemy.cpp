#include "InWanderLand.h"
#include "Enemy.h"
#include "Dotween.h"
#include "ProjectileSystem.h"
#include "Projectile.h"

void Enemy::Start()
{
	unitType = "Enemy";
	m_speed = 1.0f;
	m_bulletSpeed = 5.1f;
	chaseUpdateDelay = 1;

	isDistanceComparingStarted = false;
	
	unitFSM = new FSM<UnitState>(UnitState::Idle);

	unitFSM->transitions[UnitState::Idle].push_back({ UnitState::Chase,
		[this]() { return idleToChase == true; } });

	unitFSM->transitions[UnitState::Idle].push_back({ UnitState::Attack,
		[this]() { return idleToAttack == true; } });

	unitFSM->transitions[UnitState::Chase].push_back({ UnitState::Idle,
		[this]() { return chaseToIdle == true; } });

	unitFSM->transitions[UnitState::Attack].push_back({ UnitState::Idle,
		[this]() { return attackToIdle == true; } });

	for (int i = 1; i < UnitState::StateEnd - 1; i++)
	{
		unitFSM->transitions[UnitState(i)].push_back({ UnitState::Idle,
			[this]() {return initToIdle == true; } });
	}

	unitFSM->engageAction[UnitState::Idle] = [this]() { IdleEngage(); };
	unitFSM->engageAction[UnitState::Chase] = [this]() { ChaseEngage(); };
	unitFSM->engageAction[UnitState::Attack] = [this]() { AttackEngage(); };

	unitFSM->updateAction[UnitState::Idle] = [this]() { IdleUpdate(); };
	unitFSM->updateAction[UnitState::Chase] = [this]() { ChaseUpdate(); };
	unitFSM->updateAction[UnitState::Attack] = [this]() { AttackUpdate(); };
}

void Enemy::Update()
{
	unitFSM->UpdateState();

	// 인식 범위 내에 들어오게 된다면, 목표로 하는(적군) 오브젝트의 위치 정보를 계속 받아와야한다.
	// 그렇게 되면 이동중에 해당 적군 오브젝트의 위치가 바뀌어도 그에 맞게 자연스럽게 이동할 수 있지 않을까?
}

void Enemy::OnDestroy()
{
	delete unitFSM;
}

#pragma region TransitionFuctions
	void Enemy::IdleToOtherStatesTransition()
	{
		// Chase로 갈 것인지, Attack으로 갈 것인지 판별
		// -> idleToChase냐 idleToAttack 이냐!
		if (unitFSM->previousState == UnitState::Attack/* && !isJustEntered*/)
		{
			transitionDelay = 0.7f;
		}
		else
		{
			transitionDelay = 0.0f;
			isJustEntered = false;
		}

		GetGameObject()->GetComponent<Dotween>()->DONothing(transitionDelay).OnComplete([=]()
			{
				if (m_currentTargetObject != nullptr)
				{
					float distance = (m_currentTargetObject->GetTransform()->GetWorldPosition()
						- GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

					if (distance < m_IdDistance && distance > m_AtkDistance)
						idleToChase = true;

					else if (distance < m_AtkDistance)
						idleToAttack = true;
				}
			});
	}

	void Enemy::ChaseTransition()
	{
		chaseToIdle = true;
	}

	void Enemy::AttackToIdleTransition()
	{
		attackToIdle = true;
	}
#pragma endregion

#pragma region State Engage Functions
	void Enemy::IdleEngageFunction()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });

		GetGameObject()->GetComponent<Dotween>()->StopAllDotweenFunction();

		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(GetGameObject()->GetTransform()->GetWorldPosition());

		IdleToOtherStatesTransition();
	}

	void Enemy::AttackEngageFunction()
	{
		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(GetGameObject()->GetTransform()->GetWorldPosition());
		// 상대방을 공격하는 로직. 근접 공격 or 원거리 (투사체) 공격
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });

		// 투사체가 설정되어 있다면 원거리 공격을 한다.
		ProjectileSystem::GetInstance()->Shoot(this, m_currentTargetObject->GetComponent<Unit>(), m_bulletSpeed);

		/// 공격 후 m_Delay간격으로 재귀를 할지, 아니면 쫓아갈 지를 정한다.
		GetGameObject()->GetComponent<Dotween>()->DONothing(0.5f/*딜레이*/).OnComplete([=]()
			{
				float distance = (m_currentTargetObject->GetTransform()->GetWorldPosition() - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

				if (distance < m_AtkDistance)
				{
					AttackEngageFunction();
				}
				else
				{
					AttackToIdleTransition();
				}
			});
	}

	void Enemy::ChaseEngageFunction()
	{
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });

		float tempShortestDistance = 0.0f;

		for (auto e : m_opponentGameObjectList)
		{
			float distance = (GetGameObject()->GetTransform()->GetWorldPosition() - e->GetTransform()->GetWorldPosition()).Magnitude();

			if (!isDistanceComparingStarted || tempShortestDistance > distance)
			{
				tempShortestDistance = distance;
				m_currentTargetObject = e;
				isDistanceComparingStarted = true;
			}
		}

		GetGameObject()->GetComponent<NavigationAgent>()->MoveTo(m_currentTargetObject->GetTransform()->GetWorldPosition());

		GetGameObject()->GetComponent<Dotween>()->DONothing(chaseUpdateDelay).OnComplete([=]()
			{
				ChaseEngageFunction();
			});
	}
#pragma endregion 

void Enemy::IdleEngage()
{
	initToIdle = false;
	chaseToIdle = false;
	attackToIdle = false;
	IdleEngageFunction();
}

void Enemy::ChaseEngage()
{
	idleToChase = false;
	GetGameObject()->GetComponent<NavigationAgent>()->SetSpeed(m_speed);

	ChaseEngageFunction();
}

void Enemy::AttackEngage()
{
	idleToAttack = false;
	AttackEngageFunction();
}

void Enemy::IdleUpdate()
{

}

void Enemy::ChaseUpdate()
{
	float distance = (m_currentTargetObject->GetTransform()->GetWorldPosition()
		- GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();

	if (distance < m_AtkDistance)
	{
		ChaseTransition();
	}
}

void Enemy::AttackUpdate()
{
}


