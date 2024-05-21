#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include "ContentsObservee.h"

/// <summary>
/// 실제로 발사체에 붙여줄 컴포넌트.
/// 움직임과 다른 유닛과 충돌 했을 경우의 로직으로 구성된다.
/// </summary>

class StatusEffect;
class PlayerUnit;

class AutoAttackProjectile : public Component, public ContentsObservee
{
protected:
	float m_speed;
	Unit::UnitType ownerType;
	Unit* m_opponentUnit;
	Unit* m_ownerUnit;

	float m_range;
	bool isShootOperating = false;

	Vector3d m_ownerUnitFront;
	float localTimeScale{ 1 };

	std::vector<PlayerUnit*> m_playerUnitVector;

public:
	void Init();
	void SetOwnerType(Unit::UnitType type);
	virtual void Shoot(Unit* ownerUnit, Unit* opponentUnit, float speed, float offset);
	virtual void ReturnToPool() = 0;

	void SetStraightBulletRange(float p_rng);

private:
	void AutoChaseShootingFunction();
	void StraightShootingFunction();
	void RotateBulletPerFrame();
	void ProcessBulletHit(Unit* p_damagedUnit);

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual Component* GetComponent() override { return this; }
};

