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
        GC<float> manaRegen = 7.0f;
        GC<float> maxMana = 100.0f;

        // 카메라에 진동효과가 주어졌을 때 진동속도와 거리가 감소하는 비율, 무조건 0~1 사이의 값이어야 한다.
        GC<float> camShakeDecrementFactor = 0.5f;
        // 카메라의 이상적인 위치와 각도가 정해졌을 때 카메라의 트랜스폼이 보간되는 속도
        GC<float> cameraInterpolationSpeed = 3.0f;
        // 몇초마다 유닛들을 플레이어 캐릭터들에게 분배해주는가
        GC<float> enemyAggroUpdateInterval = 1;
        // +- 몇초씩 랜덤하게 주기에 변화를 주는가.
        GC<float> enemyAggroUpdateIntervalNoise = 1;
        // 유닛들을 플레이어들에게 분배할 때 분배할 유닛 수의 할당량
        GC<float> enemyAggroUpdateQuota = 2;
        // +- 몇명씩 랜덤하게 분배해주는가
        GC<float> enemyAggroUpdateQuotaNoise = 2;
        // 유닛들이 등장할 때 등장 시간 오프셋에 대한 노이즈
        GC<float> unitBirthTimeOffsetNoise = 0.2;

        GC<float> peaceFollowingXOffest = 3.0f;
        GC<float> peaceFollowingZOffest = 2.0f;

        GC<float> tacticSkillCost = 10.0f;
        GC<float> tacticMoveCost = 10.0f;
        GC<float> tacticAttackCost = 10.0f;

        GC<float>	tacticGaugeRecoveryPerSecond = 2.0f;

        GC<int>	maxResurrectCount;
        GC<float> chessBlockUnitLength = 3.4f;
        GC<float> chessBlockUnitOffset = 0.1f;
        GC<float> vibeMaxOffset = 0.12f;
        GC<float> defaultAnimBlendTime = 0.32f;

        GC<float> bossAppearHeight = 60.f;
        GC<float> bossAppearTime = 2.f;
        GC<float> bossSkillPeriod_1 = 4.f;
        GC<float> bossSkillPeriod_2 = 4.f;
        GC<float> bossSkillPeriod_3 = 6.f;
        GC<float> bossSkillPeriod_4 = 8.f;

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



