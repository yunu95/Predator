#pragma once
#include "Component.h"
#include "Collision2D.h"
#include "Vector3.h"
#include "Projectile.h"

class Bullet : public Projectile
{
public:
	Bullet();

	static void CreateProjectile();
	
	virtual void Initialize() override;
	virtual void Destroy() override;

	virtual void Update() override;
	virtual void OnCollisionEnter2D(const yunutyEngine::Collision2D& collision) override;
	virtual void OnCollisionExit2D(const yunutyEngine::Collision2D& collision) override;

};

