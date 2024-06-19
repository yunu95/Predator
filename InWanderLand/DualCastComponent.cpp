#include "DualCastComponent.h"
#include "Unit.h"
#include "DebuggingMeshPool.h"
#include "HealerAutoAttackProjectile.h"

void DualCastComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	auto debuggingMesh = DebuggingMeshPool::Instance().Borrow();
	debuggingMesh->SetUnitObject(opponentUnit);

	if (opponentUnit->IsPlayerUnit())
	{
		/// 이로운 효과 적용
		debuggingMesh->PopMeshUP(yunuGI::Color::blue(), MaterialNum::Green);

		/// 패시브 스택 ++
		HealerAutoAttackCount::currentPassiveCount++;
	}
	else if (opponentUnit->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		/// 해로운 효과 적용
		debuggingMesh->PopMeshUP(yunuGI::Color::black(), MaterialNum::Green);
	}
}

void DualCastComponent::OnTriggerEnter(physics::Collider* collider)
{
	// Request StatusTimer To TimerPool here
	if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
		colliderUnitComponent != nullptr)
	{
		ApplyStatus(m_ownerUnit, colliderUnitComponent);
	}
}
