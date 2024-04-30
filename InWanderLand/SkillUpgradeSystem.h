#pragma once

#include "Singleton.h"
#include "YunutyEngine.h"
#include "UIEnumID.h"

class SkillUpgradeSystem :public SingletonClass<SkillUpgradeSystem>
{
public:
    // 어떤 스킬을 업그레이드 할 것인지 미리 정한다. 미리 지정만 하는 것이지 바로 업그레이드까지 직행하는 것은 아니다.
    void SetUpgradeTarget(UIEnumID id);
    bool IsUpgraded(UIEnumID id);
    void UpgradeSkill();
    void SetSkillPoints(int points);
    int GetSkillPoints();
    void IncrementSkillPoint();
    // 업그레이드한 스킬들을 초기화한다. UI가 초기화된 이후에 부르시오.
    void Reset();
private:
    UIEnumID upgradeTarget;
    std::unordered_set<UIEnumID> upgradedList;
    int skillPointsLeft{ 5 };
};
