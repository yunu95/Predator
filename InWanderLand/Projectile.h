#pragma once
#include "Component.h"
#include "Collision2D.h"
#include "Vector3.h"
#include "Timer.h"

class Projectile : public yunutyEngine::Component
{
public:
	virtual void Initialize() = 0;
	virtual void Destroy() = 0;

	yunutyEngine::Vector3d speed;
	bool isPlayerProjectile;
	float damage;

protected:
	WanderUtil::Timer destroyAnimTimer;
	bool m_isDestroyTimerOn;
	float m_width;
	float m_height;
};

