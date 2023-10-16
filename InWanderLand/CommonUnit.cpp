#include "CommonUnit.h"
#include "Scene.h"
#include "GameObject.h"
#include "Collider2D.h"
#include "BoxCollider2D.h"
#include "CircleCollider2D.h"
#include "Transform.h"
#include "Input.h"
#include "Curve.h"
#include "YunutyTime.h"
#include "StaticMeshRenderer.h"
#include <string>
using namespace yunutyEngine;

CommonUnit::CommonUnit()
	: m_unitType(UnitType::WARRIOR),
	m_state(State::IDLE),
	lastMouseScreenPos{ 0, 0 },
	m_isPlayer(false),
	m_isDying(false),
	m_isAttackReady(true),
	m_hp(10.0f),
	m_mp(5.0f),
	m_defense(1.0f),
	m_moveSpeed(5.0f),
	m_attackPower(1.0f),
	m_attackSpeed(1.0f),
	m_attackCoolDown(0.7f),
	m_attackRange(2.0f),
	m_detectRange(4.0f),
	m_hitboxWidth(2.0f),
	m_hitboxHeight(2.0f)
{
	attackTimer.duration = 1.0f / m_attackSpeed;
	attackTimer.onTimerExpiration = [this]()
	{
		m_state = State::IDLE;
		attackCoolDownTimer.Start();
	};
	attackCoolDownTimer.duration = m_attackCoolDown;
	attackCoolDownTimer.onTimerExpiration = [this]()
	{
		m_isAttackReady = true;
	};

}

void CommonUnit::Initialize(UnitType unitType)
{
	m_unitType = unitType;
	switch (m_unitType)
	{
		case UnitType::PLAYER:
		{
			m_isPlayer = true;
			m_hp = 200.0f;
			m_mp = 50.0f;
			m_defense = 10.0f;
			m_moveSpeed = 5.0f;
			m_attackPower = 2.0f;
			m_attackSpeed = 1.0f;
			m_attackCoolDown = 0.7f;
			m_attackRange = 1.5f;
			m_detectRange = 4.0f;
			m_hitboxWidth = 1.2f;
			m_hitboxHeight = 2.0f;
		}
		break;
		case UnitType::WARRIOR:
		{
			m_hp = 20.0f;
			m_mp = 10.0f;
			m_defense = 5.0f;
			m_moveSpeed = 5.0f;
			m_attackPower = 2.0f;
			m_attackSpeed = 1.0f;
			m_attackCoolDown = 0.7f;
			m_attackRange = 1.5f;
			m_detectRange = 4.0f;
			m_hitboxWidth = 1.2f;
			m_hitboxHeight = 2.0f;
		}
		break;
		case UnitType::ARCHER:
		{
			m_hp = 14.0f;
			m_mp = 10.0f;
			m_defense = 3.0f;
			m_moveSpeed = 5.0f;
			m_attackPower = 4.0f;
			m_attackSpeed = 1.15f;
			m_attackCoolDown = 0.8f;
			m_attackRange = 5.0f;
			m_detectRange = 6.0f;
			m_hitboxWidth = 1.2f;
			m_hitboxHeight = 2.0f;
		}
		break;
		default:
			break;
	}
	attackTimer.duration = 1.0f / m_attackSpeed;
	attackCoolDownTimer.duration = m_attackCoolDown;

	// Player Hitbox
	auto playerHitbox = GetGameObject()->AddComponent<BoxCollider2D>();
	playerHitbox->SetWidth(m_hitboxWidth);
	playerHitbox->SetHeight(m_hitboxHeight);
	// Player Detection Range
	auto CommonUnitDetectCollider = GetGameObject()->AddComponent<CircleCollider2D>();
	CommonUnitDetectCollider->SetRadius(m_detectRange);
	CommonUnitDetectCollider->SetTag("Detect");
	// Player Attack Range
	auto CommonUnitAttackRange = GetGameObject()->AddComponent<CircleCollider2D>();
	CommonUnitAttackRange->SetRadius(m_attackRange);
	CommonUnitAttackRange->SetTag("Attack");

#if defined(DEBUG) || defined(_DEBUG)
	// Hitbox Debug Cube
	auto childObject = GetGameObject()->AddGameObject();
	auto hixBoxRect = childObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	hixBoxRect->GetGI().LoadMesh("Cube");
	hixBoxRect->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0,1,0,0 });
	hixBoxRect->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");
	childObject->GetTransform()->scale = Vector3d(m_hitboxWidth, m_hitboxHeight, 0);
	// Detection Range Debug Sphere
	auto childObject2 = GetGameObject()->AddGameObject();
	auto detectRangeSphere = childObject2->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	detectRangeSphere->GetGI().LoadMesh("Sphere");
	detectRangeSphere->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1,0,1,0 });
	detectRangeSphere->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");
	childObject2->GetTransform()->scale = Vector3d(m_detectRange * 2, m_detectRange * 2, 0);
	// Attack Range Debug Sphere
	auto childObject3 = GetGameObject()->AddGameObject();
	auto attackRangeSphere = childObject3->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
	attackRangeSphere->GetGI().LoadMesh("Sphere");
	attackRangeSphere->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1,0,0,0 });
	attackRangeSphere->GetGI().GetMaterial()->SetPixelShader(L"DebugPS.cso");
	childObject3->GetTransform()->scale = Vector3d(m_attackRange * 2, m_attackRange * 2, 0);
#endif
}

void CommonUnit::Idle()
{
	// Idle Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 0, 0 });
	if (m_isPlayer)
	{
		if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
		{
			// Ctrl + RightClick 입력이 들어오면 AttackMove 상태로 전환
			if (yunutyEngine::Input::isKeyDown(KeyCode::Control))
			{
				m_state = State::ATTACKMOVE;
				return;
			}
			else // Ctrl 없이 RightClick 입력만 들어왔을 때
			{
				m_state = State::MOVE;
				return;
			}
		}
	}

	// 적이 탐지되었다면 Chase 상태로 전환
	if (!detectedEnemies.empty() && attackRangeEnemies.empty())
	{
		m_state = State::CHASE;
		targetEnemy = *detectedEnemies.begin();
		return;
	}
	// 공격범위 안에 적이 있다면 Attack 상태로 전환
	if (m_isAttackReady && !attackRangeEnemies.empty())
	{
		m_state = State::ATTACK;
		attackTimer.Start();
		return;
	}
}

void CommonUnit::Move()
{
	// Ctrl + RightClick 입력이 들어오면 AttackMove 상태로 전환
	if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
	{
		if (yunutyEngine::Input::isKeyDown(KeyCode::Control))
		{
			m_state = State::ATTACKMOVE;
			return;
		}
	}
	// Move Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 0, 0 });

	// 이동할 방향으로 회전
	//Vector3d newForward = (GetTransform()->position - lastMouseScreenPos).Normalized();
	//Vector3d newRight = (Vector3d::Cross(GetTransform()->GetWorldRotation().Up(), newForward)).Normalized();
	//GetTransform()->SetWorldRotation(Quaternion::MakeWithAxes(newRight, GetTransform()->GetWorldRotation().Up(), newForward));

	// 가장 마지막으로 마우스 클릭된 위치에서 내 위치를 뺀다.
	Vector2 moveVec = lastMouseScreenPos - GetTransform()->position;
	// 마우스 클릭된 위치에 도달할 때까지 움직인다.
	moveToDestination(moveVec);
	if (moveVec.Magnitude() <= 0.03)
	{
		m_state = State::IDLE;
	}
}

void CommonUnit::AttackMove()
{
	// Ctrl 입력이 들어오지 않고 Right Click만 들어오면 Move 상태로 전환
	if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
	{
		if (!yunutyEngine::Input::isKeyDown(KeyCode::Control))
		{
			m_state = State::MOVE;
			return;
		}
	}

	// 이동하는 동안 적을 만나면 적에게 접근한다.
	if (!detectedEnemies.empty())
	{
		m_state = State::CHASE;
		return;
	}

	// Move Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 0, 0 });

	// 이동할 방향으로 회전
	//Vector3d newForward = (GetTransform()->position - lastMouseScreenPos).Normalized();
	//Vector3d newRight = (Vector3d::Cross(GetTransform()->GetWorldRotation().Up(), newForward)).Normalized();
	//GetTransform()->SetWorldRotation(Quaternion::MakeWithAxes(newRight, GetTransform()->GetWorldRotation().Up(), newForward));

	// 가장 마지막으로 마우스 클릭된 위치에서 내 위치를 뺀다.
	Vector2 moveVec = lastMouseScreenPos - GetTransform()->position;
	// 마우스 클릭된 위치에 도달할 때까지 움직인다.
	moveToDestination(moveVec);
	if (moveVec.Magnitude() <= 0.03)
	{
		m_state = State::IDLE;
	}
}

void CommonUnit::Attack()
{
	CommonUnit* enemy = nullptr;
	// attackRangeEnemies 중에 targetEnemy가 있으면 공격
	for (auto it = attackRangeEnemies.begin(); it != attackRangeEnemies.end();)
	{
		if (*it == targetEnemy)
		{
			enemy = *it;
			break;
		}
		else
		{
			++it;
		}
	}
	// 없으면 attackRangeEnemies.begin()을 공격
	if (enemy == nullptr && !attackRangeEnemies.empty())
		enemy = *attackRangeEnemies.begin();

	// 적의 방향을 향해 회전한다.
	//Vector3d newForward = (GetTransform()->position - enemy->GetTransform()->position);
	//Vector3d newRight = (Vector3d::Cross(GetTransform()->GetWorldRotation().Up(), newForward));
	//GetTransform()->SetWorldRotation(Quaternion::MakeWithAxes(newRight, GetTransform()->GetWorldRotation().Up(), newForward));

	// Attack Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });
	// Damage
	if (enemy != nullptr)
	{
		if (m_isAttackReady)
		{
			enemy->SetHP(enemy->GetHP() - m_attackPower);
			m_isAttackReady = false;
		}
	}
}

void CommonUnit::Chase()
{
	// Chase Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 1, 0 });

	if (m_isPlayer)
	{
		if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
		{
			// Ctrl + RightClick 입력이 들어오면 AttackMove 상태로 전환
			if (yunutyEngine::Input::isKeyDown(KeyCode::Control))
			{
				m_state = State::ATTACKMOVE;
				return;
			}
			else // Ctrl 없이 RightClick 입력만 들어왔을 때
			{
				m_state = State::MOVE;
				return;
			}
		}
	}

	// 추적하는 동안 적이 죽거나 탐지범위 밖으로 나가면 IDLE 상태로 바꿔준다.
	if (targetEnemy != *detectedEnemies.begin())
	{
		m_state = State::IDLE;
		return;
	}

	// 적이 있는 위치에서 내 위치를 뺀다.
	Vector3d targetEnemyPos = targetEnemy->GetTransform()->position;
	Vector2 moveVec = targetEnemyPos - GetTransform()->position;

	// 적이 있는 방향으로 회전
	//Vector3d newForward = GetTransform()->position - targetEnemyPos;
	//Vector3d newRight = Vector3d::Cross(GetTransform()->GetWorldRotation().Up(), newForward);
	//GetTransform()->SetWorldRotation(Quaternion::MakeWithAxes(newRight, GetTransform()->GetWorldRotation().Up(), newForward));

	// 적이 있는 위치에 도달할 때까지 움직인다.
	moveToDestination(moveVec);
	if (moveVec.Magnitude() <= m_attackRange + targetEnemy->GetHitBoxWidth() / 2.0)
	{
		m_state = State::IDLE;
	}
}

void CommonUnit::Die()
{
	// Die Animation
	GetGameObject()->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });
	GetGameObject()->SetSelfActive(false);
	//Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
	m_isDying = true;

	// 탐지범위 안에 있던 적들의 탐지List에서 지금 오브젝트 삭제
}

void CommonUnit::moveToDestination(Vector3d targetPos)
{
	//GetTransform()->position += targetPos.Normalized() * m_moveCurve.Evaluate(moveTimer.elapsed / moveTimer.duration) * 0.08f;
	GetTransform()->position += targetPos.Normalized() * Time::GetDeltaTime() * m_moveSpeed;
}

void CommonUnit::Update()
{
	if (m_isDying)
		return;

	attackCoolDownTimer.Update();
	if (m_hp <= 0.0f)
	{
		m_state = State::DIE;
	}

	switch (m_state)
	{
		case State::IDLE:
		{
			Idle();
		}
		break;
		case State::MOVE:
		{
			Move();
		}
		break;
		case State::ATTACKMOVE:
		{
			AttackMove();
		}
		break;
		case State::CHASE:
		{
			Chase();
		}
		break;
		case State::ATTACK:
		{
			attackTimer.Update();
			Attack();
		}
		break;
		case State::DIE:
		{
			Die();
		}
		break;
		default:
			break;
	}
}

void CommonUnit::OnCollisionEnter2D(const yunutyEngine::Collision2D& collision)
{
	if (collision.m_Collider->GetTag() == "Detect")
	{
		auto targetObject = collision.m_OtherCollider->GetGameObject();
		if (targetObject->GetComponent<CommonUnit>() != nullptr)
		{
			auto target = targetObject->GetComponent<CommonUnit>();
			if (m_isPlayer)
			{
				if (target->GetIsPlayer() == false)
					detectedEnemies.emplace_back(target);
			}
			else
			{
				if (target->GetIsPlayer() == true)
					detectedEnemies.emplace_back(target);
			}
		}
	}

	if (collision.m_Collider->GetTag() == "Attack")
	{
		auto targetObject = collision.m_OtherCollider->GetGameObject();
		if (targetObject->GetComponent<CommonUnit>() != nullptr)
		{
			auto target = targetObject->GetComponent<CommonUnit>();
			if (m_isPlayer)
			{
				if (target->GetIsPlayer() == false)
					attackRangeEnemies.emplace_back(target);
			}
			else
			{
				if (target->GetIsPlayer() == true)
					attackRangeEnemies.emplace_back(target);
			}
		}
	}
}

void CommonUnit::OnCollisionExit2D(const yunutyEngine::Collision2D& collision)
{
	if (collision.m_Collider->GetTag() == "Detect")
	{
		auto targetObject = collision.m_OtherCollider->GetGameObject();
		if (targetObject->GetComponent<CommonUnit>() != nullptr)
		{
			auto target = targetObject->GetComponent<CommonUnit>();
			for (auto it = detectedEnemies.begin(); it != detectedEnemies.end();)
			{
				if (*it == target)
				{
					detectedEnemies.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}
		}
	}

	if (collision.m_Collider->GetTag() == "Attack")
	{
		auto targetObject = collision.m_OtherCollider->GetGameObject();
		if (targetObject->GetComponent<CommonUnit>() != nullptr)
		{
			auto target = targetObject->GetComponent<CommonUnit>();
			for (auto it = attackRangeEnemies.begin(); it != attackRangeEnemies.end();)
			{
				if (*it == target)
				{
					attackRangeEnemies.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}
		}
	}
}
