#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 실제로 발사체에 붙여줄 컴포넌트.
/// 움직임과 다른 유닛과 충돌 했을 경우의 로직으로 구성된다.
/// </summary>

class Projectile : public Component
{
private:
	float m_speed = 2.0f;
	string ownerType;

public:
	void SetOwnerType(string type);

public:
	virtual void Start() override;
	virtual void OnCollisionEnter2D(const Collision2D& collision) override;
};

