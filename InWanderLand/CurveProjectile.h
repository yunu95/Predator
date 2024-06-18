#pragma once
#include "Projectile.h"
#include "ProjectilePool.h"

class CurveProjectile : public Projectile
{
private:
	virtual void Update() override;

	virtual void OnContentsStop() override;

public:
	virtual void Init() override;
	friend ProjectilePool<CurveProjectile>;
};