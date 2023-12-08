#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 실제로 발사체에 붙여줄 컴포넌트.
/// 움직임과 다른 유닛과 충돌 했을 경우의 로직으로 구성된다.
/// </summary>

class Unit;

class Projectile : public Component
{
private:
	float m_speed;
	string ownerType;
	Unit* m_opponentUnit;
	Unit* m_ownerUnit;

	bool isShootStarted = false;
	bool isPassed;

public:
	void SetOwnerType(string type);

	void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed);
	void ShootFunction();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnCollisionEnter2D(const Collision2D& collision) override;
};

