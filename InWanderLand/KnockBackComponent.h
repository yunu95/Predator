#pragma once
#include "StatusEffectTrigger.h"
#include <list>
/// <summary>
/// 피격 유닛을 밀어내도록 하는 컴포넌트, Robin의 돌진 구현에 쓰인다.
/// </summary>

class StatusTimer;

class KnockBackComponent : public StatusEffectTrigger
{
private:
    // 중복 충돌을 방지하기 위한 멤버
    std::list<Unit*> crushedUnitList;
    bool isSkillStarted;

public:
    virtual void ApplyStatus(Unit* ownerUnit, Unit* opponentUnit) override;
    float pushDistance = 8.5f;
    float pushDuration = 0.5f;
    static constexpr float maxKnockHeight = 4;
    float m_damage = 1.0f;

public:
    virtual void Update() override;

public:
    void SetAP(float p_ap);
    void SkillStarted();
    void ClearCrushedUnitList();
};

