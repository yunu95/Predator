#pragma once
#include "FieldDamage.h"

class StatusTimer;

/// <summary>
/// 우르술라 2번 스킬.
/// 적군을 끌어당김 -> 경직 부여
/// 경직 해제 후 장판 위에 있을 시 이동 속도 저하.
/// </summary>
class ParalysisFieldComponent : public FieldDamage
{
public:
    physics::SphereCollider* m_collider{ nullptr };
    virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;

private:
    StatusTimer* m_paralysisTimer;

    std::unordered_map<Unit*, bool> m_onFieldUnitsMap;

    float m_paralysisTime;
    float m_slowMultipleScale;
    float m_pullingPower;
    float m_pullingTime;

    virtual void SetFieldSkillMembers() override;

public:
    virtual void OnTriggerExit(physics::Collider* collider) override;
};

