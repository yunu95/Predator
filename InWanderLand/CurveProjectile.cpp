#include "CurveProjectile.h"

void CurveProjectile::Init()
{
	Vector3d horizontalDisplacement = opponentUnit.lock()->GetTransform()->GetWorldPosition() - owner.lock()->GetTransform()->GetWorldPosition();
	horizontalDisplacement.y = 0;
	this->speed = horizontalDisplacement.Normalized() * owner.lock()->GetUnitTemplateData().pod.projectileSpeed;
	float t = horizontalDisplacement.Magnitude() / this->speed.Magnitude();
	float y0 = owner.lock()->GetTransform()->GetWorldPosition().y;
	float vy0 = (-y0 + 0.5f * GlobalConstant::GetSingletonInstance().pod.gravitySpeed * t * t) / t;

	this->speed += Vector3d::up * vy0;
	this->damage = owner.lock()->GetUnitTemplateData().pod.m_autoAttackDamage + owner.lock()->adderAttackDamage;
	this->traveling = true;
}

void CurveProjectile::Update()
{
	UnitAcquisitionSphereCollider::Update();
	if (traveling)
	{
		if (!enemies.empty())
		{
			if (!owner.lock()->referenceBlindness.BeingReferenced())
			{
				(*enemies.begin())->Damaged(owner, damage, DamageType::Attack);
			}
			ProjectilePool<CurveProjectile>::SingleInstance().Return(GetWeakPtr<CurveProjectile>());
			return;
		}

		GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + speed * Time::GetDeltaTime());
		SetSpeed(speed + Vector3d::down * GlobalConstant::GetSingletonInstance().pod.gravitySpeed * Time::GetDeltaTime());
		if (GetTransform()->GetWorldPosition().y < 0)
		{
			ProjectilePool<CurveProjectile>::SingleInstance().Return(GetWeakPtr<CurveProjectile>());
		}
	}
}

void CurveProjectile::OnContentsStop()
{
	ProjectilePool<CurveProjectile>::SingleInstance().Return(GetWeakPtr<CurveProjectile>());
}
