#include "AutoAttackProjectile.h"
#include "InWanderLand.h"
#include "Dotween.h"
#include "AutoAttackProjectilePool.h"
#include "Unit.h"
#include "PlayerUnit.h"
#include "StatusEffect.h"
#include "PlayerController.h"
#include <float.h>
#include <cmath>

void AutoAttackProjectile::SetOwnerType(Unit::UnitType type)
{
	ownerType = type;
}

void AutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset)
{
	m_speed = speed;
	m_ownerUnit = ownerUnit;
	m_opponentUnit = opponentUnit;
	GetGameObject()->GetTransform()->SetWorldPosition(ownerUnit->GetGameObject()->GetTransform()->GetWorldPosition() + (ownerUnit->GetTransform()->GetWorldRotation().Forward() * -1 * ownerUnit->GetAttackOffset()));
	GetGameObject()->GetComponent<Dotween>()->
		DOLookAt(m_opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition(), localTimeScale * Time::GetDeltaTime(), false).OnComplete([=]()
			{
				GetGameObject()->SetSelfActive(true);
			});
	//RotateBulletPerFrame();
	Vector3d directionVector = (opponentUnit->GetTransform()->GetWorldPosition() - ownerUnit->GetTransform()->GetWorldPosition()).Normalized();
	//GetGameObject()->GetTransform()->SetWorldRotation(directionVector);

	m_ownerUnitFront = m_ownerUnit->GetTransform()->GetWorldRotation().Forward();
	isShootOperating = true;
}

void AutoAttackProjectile::SetStraightBulletRange(float p_rng)
{
	m_range = p_rng;
}

void AutoAttackProjectile::AutoChaseShootingFunction()
{
	//// 움직이기 전의 투사체 위치
	Vector3d startPosition = GetGameObject()->GetTransform()->GetWorldPosition();

	Vector3d endPosition = m_opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition();
	float distance = (endPosition - startPosition).Magnitude();

	// 움직이기 전 방향 벡터
	Vector3d directionVector = (endPosition - startPosition).Normalized();

	// 움직이고 나서의 투사체 위치.
	Vector3d movedPositionPerFrame = GetGameObject()->GetTransform()->GetWorldPosition() + (directionVector * m_speed * localTimeScale * Time::GetDeltaTime());

	// 움직이고 나서의 투사체 -> 목표물 방향 벡터
	Vector3d afterDirectionVector = (endPosition - movedPositionPerFrame).Normalized();

	GetGameObject()->GetTransform()->SetWorldPosition(movedPositionPerFrame);

	float dotProducted = Vector3d::Dot(directionVector, afterDirectionVector);
	float multipledLength = directionVector.Magnitude() * afterDirectionVector.Magnitude();

	float angle = dotProducted / multipledLength;

	if (angle < 0)
	{
		ProcessBulletHit(m_opponentUnit);
	}

	//RotateBulletPerFrame();
	GetGameObject()->GetComponent<Dotween>()->DOLookAt(m_opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition(), localTimeScale * Time::GetDeltaTime(), false);
}

void AutoAttackProjectile::StraightShootingFunction()
{
	Vector3d startPosition = GetGameObject()->GetTransform()->GetWorldPosition();
	Vector3d endPosition = m_ownerUnit->GetGameObject()->GetTransform()->GetWorldPosition()
		+ m_ownerUnit->GetTransform()->GetWorldRotation().Forward() * -1 * m_range;

	Vector3d movedPositionPerFrame = GetGameObject()->GetTransform()->GetWorldPosition() + m_ownerUnitFront * -1 * localTimeScale * Time::GetDeltaTime() * m_speed;

	GetGameObject()->GetTransform()->SetWorldPosition(movedPositionPerFrame);

	if ((GetTransform()->GetWorldPosition() - m_ownerUnit->GetTransform()->GetWorldPosition()).Magnitude() > m_range)
	{
		GetGameObject()->SetSelfActive(false);

		//AutoAttackProjectilePool::Instance().Return(this);
		ReturnToPool();
		isShootOperating = false;
	}

	else
	{
		for (auto each : m_playerUnitVector)
		{
			float distance = (each->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition()).Magnitude();

			if (distance < 1.0f)
			{
				ProcessBulletHit(each);
				break;
			}
		}
	}
}

void AutoAttackProjectile::RotateBulletPerFrame()
{
	Vector3d startPosition = GetGameObject()->GetTransform()->GetWorldPosition();
	Vector3d endPosition = m_opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition();
	Vector3d objectFront = GetGameObject()->GetTransform()->GetWorldRotation().Forward();
	Vector3d distanceVec = endPosition - startPosition;

	double dot = Vector3d::Dot(objectFront, startPosition - endPosition);

	double angle;
	double sq;
	double finalAngle;
	double finalDegree;

	// 회전 방향 판정
	Vector3d axis = Vector3d::Cross(objectFront, distanceVec);

	angle = (objectFront.x * distanceVec.x + objectFront.z * distanceVec.z);
	sq = (sqrt(pow(objectFront.x, 2) + pow(objectFront.z, 2)) *
		sqrt(pow(distanceVec.x, 2) + pow(distanceVec.z, 2)));

	// 두 벡터의 각도가 180도 이상이면 180을, -180 이하 이라면 -180을 
	//finalAngle = acos( max( -1.0f, min(1.0f, angle / sq) ) );
	finalAngle = acos(std::clamp(angle / sq, -1.0, 1.0));			// c++17 된다면
	finalDegree = 57.2969f * (finalAngle);

	if (axis.y < 0)
		finalDegree *= -1;

	if (abs(finalDegree) > 0.05)
		GetGameObject()->GetTransform()->SetWorldRotation(Quaternion({ 0.0f, finalDegree, 0.0f }));

}

void AutoAttackProjectile::ProcessBulletHit(Unit* p_damagedUnit)
{
	GetGameObject()->SetSelfActive(false);

	//AutoAttackProjectilePool::Instance().Return(this);
	ReturnToPool();

	/// 충돌 (목적지 도착 시) 호출하고자 하는 로직은 여기에
	p_damagedUnit->Damaged(m_ownerUnit, m_ownerUnit->DetermineAttackDamage(m_ownerUnit->GetUnitDamage()));

	isShootOperating = false;
}
void AutoAttackProjectile::Init()
{
	for (auto each : PlayerController::Instance().GetPlayerMap())
	{
		m_playerUnitVector.push_back(each.second);
	}
}
void AutoAttackProjectile::Start()
{
	/*if (!isShootOperating)
		GetGameObject()->SetSelfActive(false);*/
}

void AutoAttackProjectile::Update()
{
	if (isShootOperating)
	{
		if (m_ownerUnit->GetUnitSide() == Unit::UnitSide::Player)
			AutoChaseShootingFunction();
		else
			StraightShootingFunction();
	}
}

