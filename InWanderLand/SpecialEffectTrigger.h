#pragma once
#include "SpecialEffect.h"

/// <summary>
/// special effect를 콜라이더에 충돌 했을 때 적용할 수 있도록 하는 컴포넌트.
/// </summary>
class SpecialEffectTrigger : public SpecialEffect
{
public:
	virtual void OnTriggerEnter(physics::Collider* collider) override;
};

