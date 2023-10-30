#include "Projectile.h"
#include "Dotween.h"
#include "ProjectileSystem.h"
#include "Unit.h"

void Projectile::SetOwnerType(string type)
{
	ownerType = type;
}

void Projectile::Start()
{
	m_speed = 2.0f;
}

void Projectile::OnCollisionEnter2D(const Collision2D& collision)
{
	if (collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>() != nullptr && 
		collision.m_OtherCollider->GetGameObject()->GetComponent<Unit>()->GetType() != ownerType)
	{
		ProjectileSystem::GetInstance()->ReturnToPool(GetGameObject());
	}
}
