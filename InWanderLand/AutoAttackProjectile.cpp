#include "AutoAttackProjectile.h"
#include "Dotween.h"
#include "AutoAttackProjectilePool.h"
#include "Unit.h"
#include "UnitStatusComponent.h"

void AutoAttackProjectile::SetOwnerType(Unit::UnitType type)
{
	ownerType = type;
}

void AutoAttackProjectile::Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed)
{
	m_speed = speed;
	m_ownerUnit = ownerUnit;
	m_opponentUnit = opponentUnit;
	
	GetGameObject()->GetTransform()->SetWorldPosition(ownerUnit->GetGameObject()->GetTransform()->GetWorldPosition());

	isShootOperating = true;
}

void AutoAttackProjectile::ShootUpdateFunction()
{
	//// 움직이기 전의 투사체 위치
	Vector3d startPosition = GetGameObject()->GetTransform()->GetWorldPosition();

	Vector3d endPosition = m_opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition();
	float distance = (endPosition - startPosition).Magnitude();

	// 움직이기 전 방향 벡터
	Vector3d directionVector = (endPosition - startPosition).Normalized();

	// 움직이고 나서의 투사체 위치.
	Vector3d movedPositionPerFrame = GetGameObject()->GetTransform()->GetWorldPosition() + (directionVector * m_speed * Time::GetDeltaTime());
	
	// 움직이고 나서의 투사체 -> 목표물 방향 벡터
	Vector3d afterDirectionVector = (endPosition - movedPositionPerFrame).Normalized();

	GetGameObject()->GetTransform()->SetWorldRotation(directionVector);
	GetGameObject()->GetTransform()->SetWorldPosition(movedPositionPerFrame);

	float dotProducted = Vector3d::Dot(directionVector, afterDirectionVector);
	float multipledLength = directionVector.Magnitude() * afterDirectionVector.Magnitude();

	float angle = dotProducted / multipledLength;

	float finalAngle = acos(angle);

	if (angle < 0)
	{
		GetGameObject()->SetSelfActive(false);

		AutoAttackProjectilePool::SingleInstance().Return(this);

		/// 충돌 (목적지 도착 시) 호출하고자 하는 로직은 여기에
		m_opponentUnit->Damaged(m_ownerUnit->GetGameObject(), m_ownerUnit->DetermineAttackDamage(m_ownerUnit->GetUnitDamage()));

		isShootOperating = false;
	}
}

void AutoAttackProjectile::Start()
{

}

void AutoAttackProjectile::Update()
{
	if (isShootOperating)
	{
		ShootUpdateFunction();
	}
}

