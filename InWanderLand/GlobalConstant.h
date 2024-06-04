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

        GC<float>	playerOneQSkillDelay = 0.0f;
        GC<int>		playerOneQSkillTimingFrame = 10;
        GC<float>	playerOneWSkillDelay = 0.0f;
        GC<int>		playerOneWSkillTimingFrame = 10;

        GC<float>	playerTwoQSkillDelay = 0.0f;
        GC<int>		playerTwoQSkillTimingFrame = 10;
        GC<float>	playerTwoWSkillDelay = 0.0f;
        GC<int>		playerTwoWSkillTimingFrame = 10;

        GC<float>	playerThreeQSkillDelay = 0.0f;
        GC<int>		playerThreeQSkillTimingFrame = 10;
        GC<float>	playerThreeWSkillDelay = 0.0f;
        GC<int>		playerThreeWSkillTimingFrame = 10;

        GC<float>	eliteMeleeEnemySkillDelay = 0.0f;
        GC<int>		eliteMeleeEnemySkillTimingFrame = 0;
        GC<float>	eliteRangedEnemySkillDelay = 0.0f;
        GC<int>		eliteRangedEnemySkillTimingFrame = 0;

        GC<float>	bossSkillOneDelay = 0.0f;
        GC<int>		bossSkillOneTimingFrame = 15;
        GC<float>	bossSkillTwoDelay = 0.0f;
        GC<int>		bossSkillTwoTimingFrame = 15;
        GC<float>	bossSkillThreeDelay = 0.0f;
        GC<int>		bossSkillThreeTimingFrame = 15;
        GC<float>	bossSkillFourDelay = 0.0f;
        GC<int>		bossSkillFourTimingFrame = 15;

        GC<float>	spikeTrapSkillDelay = 3.0f;
        GC<float>	chessTrapSkillDelay = 2.0f;
        GC<float>	triggeredTrapSkillDelay;
        GC<int>		triggeredTrapSkillTimingFrame;

        GC<float>	robinQSkillCoolTime = 2.0f;
        GC<float>	robinQSkillCost = 10.0f;
        GC<float>	robinQSkillMaxDistance = 8.0f;
        GC<float>	robinQSkillRushSpeed = 20.0f;
        GC<float>	robinQSkillDamageRush = 20.0f;
        GC<float>	robinQSkillDamageImpact = 80.0f;
        GC<float>	robinQSkillRushKnockbackDistance = 3.0f;
        GC<float>	robinQSkillImpactKnockbackDistance = 8.0f;
        GC<float>	robinQSkillRushKnockbackDuration = 0.5f;
        GC<float>	robinQSkillImpactKnockbackDuration = 1.0f;
        GC<float>	robinQSkillImpactStunDuration = 2.0f;
        GC<float>	robinQSkillRushKnockbackRadius = 1.0f;
        GC<float>	robinQSkillImpactKnockbackRadius = 5.0f;
        GC<float>	robinESkillCoolTime = 2.0f;
        GC<float>	robinESkillCost = 10.0f;
        GC<float>	robinESkillRadius = 6.0f;

        GC<float>	ursulaQSkillCoolTime = 2.0f;
        GC<float>	ursulaQSkillCost = 10.0f;
        GC<float>	ursulaQSkillRadius = 3;
        GC<float>	ursulaQSkillProjectileSpeed = 15;
        GC<float>	ursulaQSkillBlindDuration = 3.0f;
        GC<float>	ursulaQSkillFieldDuration = 3.0f;
        GC<float>	ursulaQSkillFieldDamagePerTick = 1.0f;
        GC<float>	ursulaQSkillFieldDamageInterval = 0.3f;
        GC<float>	ursulaESkillCoolTime = 2.0f;
        GC<float>	ursulaESkillCost = 10.0f;
        GC<float>	ursulaESkillDamageInterval = 0.3f;
        GC<float>	ursulaESkillDamagePerTick = 1.0f;
        GC<float>	ursulaESkillStunDuration = 1.0f;
        GC<float>	ursulaESkillPullingDuration = 0.5f;
        GC<float>	ursulaESkillRadius = 4;

        GC<float>	hanselQSkillCoolTime = 2.0f;
        GC<float>	hanselQSkillCost = 10.0f;
        GC<int>	    hanselQSkillStompRepeat = 4;
        GC<float>   hanselQSkillStompDamage = 10.0f;
        GC<float>   hanselQSkillStompRadius = 10.0f;
        GC<float>	hanselESkillCoolTime = 2.0f;
        GC<float>	hanselESkillCost = 10.0f;
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
        GC<float> chessSummonedExplosionDelay = 1.5f;
        GC<float> vibeMaxOffset = 0.12f;

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



