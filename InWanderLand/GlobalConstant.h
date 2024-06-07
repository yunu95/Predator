/// 2024. 03. 11 김상준
/// Content Layer 에서 사용할 공용 변수들에 대해 관리하게 될
/// Global Constant 용 POD 구조체

#pragma once

#include "Singleton.h"
#include "GCTemplate.h"
#include "Storable.h"
#include "PodStructs.h"

namespace application
{
    struct POD_GlobalConstant
    {
        GC<int> gravitySpeed = 10;
        GC<int> magicPower = 40;

        // 몇초마다 유닛들을 플레이어 캐릭터들에게 분배해주는가
        GC<float> enemyAggroUpdateInterval = 1;
        // +- 몇초씩 랜덤하게 주기에 변화를 주는가.
        GC<float> enemyAggroUpdateIntervalNoise = 1;
        // 유닛들을 플레이어들에게 분배할 때 분배할 유닛 수의 할당량
        GC<float> enemyAggroUpdateQuota = 2;
        // +- 몇명씩 랜덤하게 분배해주는가
        GC<float> enemyAggroUpdateQuotaNoise = 2;

        GC<float>   impaleSkillDamage = 20.0f;
        GC<float>   impaleSkillCoolTime = 2.0f;
        GC<float>   impaleSkillRange = 8.0f;
        GC<float>   impaleSkillWidth = 2.0f;
        GC<float>   impaleSkillFirstSpearOffset = 0.5f;
        GC<float>   impaleSkillMaxHeightPerSpear = 2.5f;
        GC<float>   impaleSkillMinHeightPerSpear = 0.0f;
        GC<float>   impaleSkillDurationPerSpear = 0.5f;
        GC<float>   impaleSkillAriseDistancePerSpear = 0.5f;
        GC<float>   impaleSkillAriseTimeNoisePerSpear = 0.5f;
        GC<float>   impaleSkillAriseDistanceNoisePerSpear = 0.5f;
        GC<float>   impaleSkillDuration = 2.0f;
        GC<float>   impaleSkillKnockbackDistance = 2.0f;
        GC<float>   impaleSkillKnockbackDuration = 2.0f;

        GC<float>	skillGaugeRecoveryPerSecond = 2.0f;


        GC<float>	tacticSkillCost = 10.0f;
        GC<float>	tacticMoveCost = 10.0f;
        GC<float>	tacticAttackCost = 10.0f;

        GC<float>	tacticGaugeRecoveryPerSecond = 2.0f;

        GC<int>	maxResurrectCount;
        GC<float> chessBlockUnitLength = 3.4f;
        GC<float> chessBlockUnitOffset = 0.1f;
        GC<float> vibeMaxOffset = 0.12f;
        GC<float> defaultAnimBlendTime = 0.32f;

        TO_JSON(POD_GlobalConstant)
            FROM_JSON(POD_GlobalConstant)
    };

    class GlobalConstant
        : public Singleton<GlobalConstant>
    {
        friend class Singleton<GlobalConstant>;
    public:
        POD_GlobalConstant pod;

        bool PreEncoding(json& data) const;
        bool PreDecoding(const json& data);

    private:
        GlobalConstant() = default;
    };
}



