#pragma once
#include "YunutyEngine.h"
#include <stack>
/// <summary>
/// Unit의 멤버객체인 발사체 시스템 클래스.
/// 시스템이라는 이름에 걸맞게 역할은 발사체 등록 및 사격 함수만 호출
/// 발사체가 움직이고 충돌하는 로직은 Projectile 클래스에서 관리하자.
/// </summary>

class Unit;
class Projectile;

class ProjectileSystem
{
public:
	ProjectileSystem(Unit* OwnerUnit);

private:
	Unit* m_ownerUnit;
	std::stack<Projectile*> projectileStack;

public:
	void SetProjectile(Projectile* p_projectile);
	void Shoot(Vector3d enemyPosition);
	void ReturnToStack(Projectile* usedProjectile);
	string GetUnitType() const;
	bool Empty() const;
};

