#include "Zealot.h"
#include "GameObject.h"
#include "Collider2D.h"
#include "Transform.h"
#include "Input.h"
#include "Curve.h"
#include "YunutyTime.h"
#include "Zergling.h"
#include "StaticMeshRenderer.h"
using namespace yunutyEngine;

Zealot::Zealot()
	: m_state(State::IDLE), m_isMove(false), m_isChase(false), m_isAttack(false),
	m_moveSpeed(.2f), m_attackRange(1.5f),
	targetEnemyPos{ 0, 0, 0 }, lastMouseScreenPos{ 0, 0 }
{

}

void Zealot::Idle()
{
	// Idle Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().SetColor(0, yunuGI::Color{ 0, 1, 0, 0 });
	// 사용자의 이동 명령 입력이 들어오면 Move 상태로 전환
	if (yunutyEngine::Input::isKeyDown(KeyCode::MouseLeftClick))
	{
		m_state = State::MOVE;
		m_isMove = true;
		// 마우스 클릭된 위치를 받는다.
		//lastMouseScreenPos = yunutyEngine::Input::getMouseScreenPosition();
	}
	// 적을 추격 중이었다면 Chase 상태로 전환
	if (m_isChase)
	{
		m_state = State::CHASE;
	}
	// 공격범위 안에 적이 있다면 Attack 상태로 전환
}

void Zealot::Move()
{
	// Move Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().SetColor(0, yunuGI::Color{ 1, 1, 0, 0 });

	if (yunutyEngine::Input::isKeyDown(KeyCode::MouseLeftClick))
	{
		// 마우스 클릭된 위치를 받는다.
		//lastMouseScreenPos = yunutyEngine::Input::getMouseScreenPosition();
		m_isMove = true;
		//m_isChase = false;

		m_moveCurveElapsed = 0.0f;
		// 이동 모션 재생할 시간 계산. 거리 = 속력 * 시간
		m_moveCurveDuration = (lastMouseScreenPos - GetTransform()->position).Magnitude() / m_moveSpeed;
	}
	if (m_isMove)
	{
		// 가장 마지막으로 마우스 클릭된 위치에서 내 위치를 뺀다.
		Vector2 moveVec = lastMouseScreenPos - GetTransform()->position;
		// 마우스 클릭된 위치에 도달할 때까지 움직인다.
		moveToDestination(moveVec);
		if (moveVec.Magnitude() <= 0.05)
		{
			m_state = State::IDLE;
			m_isMove = false;
		}
	}
}

void Zealot::Attack()
{
	if (m_isAttack)
	{
		// Attack Animation
		GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().SetColor(0, yunuGI::Color{ 1, 0, 0, 0 });
		// Damage
	}
	// 공격모션 끝나면 IDLE로 전환해줘야 함.
}

void Zealot::Chase()
{
	// Chase Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().SetColor(0, yunuGI::Color{ 1, 0, 1, 0 });
	if (m_isChase && !m_isMove)
	{
		// 적이 있는 위치에서 내 위치를 뺀다.
		Vector2 moveVec = targetEnemyPos - GetTransform()->position;
		// 적이 있는 위치에 도달할 때까지 움직인다.
		moveToDestination(moveVec);
		if (moveVec.Magnitude() <= m_attackRange)
		{
			m_state = State::ATTACK;
			m_isChase = false;
			m_isAttack = true;
		}
	}
	// Chase 상태에 있는 동안 사용자의 이동 명령이 들어왔을 때
	if (yunutyEngine::Input::isKeyDown(KeyCode::MouseLeftClick))
	{
		// 마우스 클릭된 위치를 받는다.
		//lastMouseScreenPos = yunutyEngine::Input::getMouseScreenPosition();
		m_state = State::MOVE;
		m_isMove = true;
	}
	// 적을 추격하는 동안 적이 죽으면 Chase상태에서 빠져나가야 함.
}

void Zealot::Die()
{
	// Die Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().SetColor(0, yunuGI::Color{ 1, 1, 1, 0 });
}

void Zealot::moveToDestination(Vector3d targetPos)
{
	m_moveCurveElapsed += Time::GetDeltaTime();
	if (m_moveCurveElapsed > m_moveCurveDuration)
		m_moveCurveElapsed = m_moveCurveDuration;
	GetTransform()->position += targetPos.Normalized() * m_moveCurve.Evaluate(m_moveCurveElapsed / m_moveCurveDuration) * m_moveSpeed;
}

void Zealot::Update()
{
	switch (m_state)
	{
		case State::IDLE:
			Idle();
			break;
		case State::MOVE:
			Move();
			break;
		case State::CHASE:
			Chase();
			break;
		case State::ATTACK:
			Attack();
			break;
		case State::DIE:
			Die();
			break;
		default:
			break;
	}
}

void Zealot::OnCollisionEnter2D(const yunutyEngine::Collision2D& collision)
{
	auto targetObject = collision.m_OtherCollider->GetGameObject();
	if (targetObject->GetComponent<Zergling>() != nullptr)
	{
		if (!m_isChase)
		{
			targetEnemyPos = targetObject->GetTransform()->GetWorldPosition();
			//targetEnemyPos.x = targetEnemyPos.x - m_attackRange;
			m_state = State::CHASE;
			m_isChase = true;
			m_isMove = false;
		}
	}
}
