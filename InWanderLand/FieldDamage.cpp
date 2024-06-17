#include "FieldDamage.h"
#include "StatusTimer.h"
#include "StatusTimerPool.h"
#include "Unit.h"

void FieldDamage::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
    m_currentOnFieldUnits.insert(opponentUnit);
}

void FieldDamage::Start()
{
    SetFieldSkillMembers();
    //m_damageTimer = StatusTimerPool::Instance().Borrow();
}

void FieldDamage::Update()
{
    m_fieldDamageElapsed += Time::GetDeltaTime();

    if (m_fieldDamageElapsed >= m_fieldDamageDelay)
    {
        if (!m_currentOnFieldUnits.empty())
        {
            for (auto e : m_currentOnFieldUnits)
            {
                e->Damaged(m_fieldDamage);
            }
        }

        m_fieldDamageElapsed -= m_fieldDamageDelay;
    }
}

void FieldDamage::OnTriggerExit(physics::Collider* collider)
{
    if (Unit* colliderUnitComponent = UnitCollider::AcquireUnit(collider);
        colliderUnitComponent != nullptr &&
        colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Enemy)
    {
        m_currentOnFieldUnits.erase(colliderUnitComponent);
    }
}

void FieldDamage::ActivateFieldTimer()
{
    m_damageTimer->ActivateTimer();
}

