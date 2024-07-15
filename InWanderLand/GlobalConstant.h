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
        GC<float> manaRegen_Peace = 70.0f;
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
        // 카이팅 하는 적은 플레이어가 얼마나 가까워질 때 도주를 시도하는가.
        GC<float> enemyKitingDistanceThreshold = 4;
        // 카이팅 하는 적은 얼마나 잦은 간격으로 판단을 내리는가.
        GC<float> enemyKitingCooltime = 2;
        // 포위망을 좁혀오는 적들은 몇 초 간격으로 사거리를 줄이는가
        GC<float> enemyApproachingDecayCooltime = 4;
        // 포위망을 좁혀오는 적들은 한번에 몇%씩 사거리를 줄이는가
        GC<float> enemyApproachingDecayRate = 0.9f;
        // 포위망을 좁혀오는 적들은 최종적으로 몇%만큼 사거리를 줄이는가
        GC<float> enemyApproachingDecayEndRate = 0.4f;
        // 유닛들이 등장할 때 등장 시간 오프셋에 대한 노이즈
        GC<float> unitBirthTimeOffsetNoise = 0.2;
        // 유닛 한 기가 피해를 받았을 때 띄울 숫자 UI의 최대 갯수
        GC<int> maxDmgIndicatorCount{ 2 };
        // 숫자로 피해량을 띄울 최소 데미지
        GC<float> dmgIndicatorMinDamage{ 5.0f };
        // 피해를 받았을 때 UI의 크기는 dmgToScaleMultiplier * dmg^dmgSizeExponential 만큼 커진다.
        GC<float> dmgToScalePower{ 0.3f };
        GC<float> dmgToScaleMultiplier{ 0.3f };
        // 피해를 받았을 때 순간적으로 dmgIndicatorPulseEnlargingScale만큼 커졌다 다시 돌아온다.
        GC<float> dmgIndicatorPulseEnlargingScale{ 1.2f };
        GC<float> dmgIndicatorPulseEnlargingDuration{ 0.4f };

        GC<float> peaceFollowingXOffest = 3.0f;
        GC<float> peaceFollowingZOffest = 2.0f;
        GC<float> comboTimeLimit{ 5.0 };

        GC<float> tacticSkillCost = 10.0f;
        GC<float> tacticMoveCost = 10.0f;
        GC<float> tacticAttackCost = 10.0f;
        GC<float> tacticCameraLerpTime = 2.0f;

        GC<float> tacticGaugeRecoveryPerSecond = 2.0f;
        GC<float> battleZoomMultiplier = 0.95f;
        GC<float> tacticZoomMultiplier = 1.05f;

        GC<int>	maxResurrectCount;
        GC<float> chessBlockUnitLength = 3.4f;
        GC<float> chessBlockUnitOffset = 0.1f;
        GC<float> vibeMaxOffset = 0.12f;

        GC<float> bossAppearHeight = 60.f;
        GC<float> bossAppearTime = 2.f;
        GC<float> bossSkillPeriod_1 = 4.f;
        GC<float> bossSkillPeriod_2 = 4.f;
        GC<float> bossSkillPeriod_3 = 6.f;
        GC<float> bossSkillPeriod_4 = 8.f;

        GC<float> soundRolloffScale = 0.01f;

        GC<float> camOutsideUIMinX = 150.0f;
        GC<float> camOutsideUIMinY = 150.0f;
        GC<float> camOutsideUIMaxX = 1920.0f - 150.0f;
        GC<float> camOutsideUIMaxY = 1080.0f - 400.0f;

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



