#pragma once
#include "YunutyEngine.h"
#include <stack>
#include <vector>
/// <summary>
/// Unit의 멤버객체인 발사체 시스템 클래스.
/// 시스템이라는 이름에 걸맞게 역할은 발사체 등록 및 사격 함수만 호출
/// 발사체가 움직이고 충돌하는 로직은 Projectile 클래스에서 관리하자.
/// </summary>

class Unit;
class Projectile;
class UnitStatusComponent;

class ProjectileSystem
{
private:
	ProjectileSystem();
	~ProjectileSystem();
	static ProjectileSystem* instance;

public:
	static ProjectileSystem* GetInstance();

private:
	std::stack<GameObject*> m_projectileStack;								// 발사 대기 중인 투사체들.

public:
	void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed);
	void ReturnToPool(GameObject* usedObject);
	void SetUp();

};

