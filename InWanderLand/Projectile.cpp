#include "Projectile.h"
#include "Dotween.h"
#include "ProjectileSystem.h"
#include "Unit.h"

void Projectile::Start()
{
	isHit = false;
	m_speed = 2.0f;
}

void Projectile::OnCollisionEnter2D(const Collision2D& collision)
{
	if (collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>() != nullptr && 
		collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>()->GetType() != m_projectileSystem->GetUnitType())
	{
		m_projectileSystem->ReturnToStack(this);
		isHit = false;
	}
}

void Projectile::Shoot(Vector3d endPosition)
{
	float distance = (endPosition - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
	
	GetGameObject()->GetComponent<Dotween>()->DOMove(endPosition, distance / m_speed).OnComplete([=]()
		{
			if (!isHit)
			{
				m_projectileSystem->ReturnToStack(this);
			}
		});
}

void Projectile::SetSystem(ProjectileSystem* sys)
{
	m_projectileSystem = sys;
}
