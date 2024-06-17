#include "CurveHomingProjectile.h"

void CurveHomingProjectile::Update()
{
}

void CurveHomingProjectile::OnContentsStop()
{
	ProjectilePool<CurveHomingProjectile>::SingleInstance().Return(GetWeakPtr<CurveHomingProjectile>());
}

void CurveHomingProjectile::Init()
{
}
