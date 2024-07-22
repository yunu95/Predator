/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 유닛

#pragma once

#include "ITemplateData.h"
#include "GlobalConstant.h"
#include "UnitStatusBarType.h"
#include "PlayerCharacterType.h"
#include "UnitAttackType.h"
#include "UnitControllerType.h"
#include "ProjectileType.h"
#include "UnitDamageFontType.h"

#include <string>

namespace application
{
    namespace editor
    {
        class Unit_TemplateData;

        struct POD_Unit_TemplateData
        {
            std::string skinnedFBXName = std::string();
            // 플레이어 유닛인지의 여부와 종류를 나타내는 열거형
            POD_Enum<PlayerCharacterType::Enum> playerUnitType;
            // 추가 목숨의 갯수
            int liveCount{ 0 };
            // 부활까지 걸리는 시간
            float revivalDuration{ 10 };
            // 부활후 무적시간
            float revivalInvulnerableDuration{ 3 };
            // aggroRatio가 높을수록 적의 이목을 더 잘 끈다.
            float playerAggroRatio{ 1.0f };
            // 화면에 뜨는 체력창의 타입
            POD_Enum<UnitStatusBarType::Enum> unitStatusBar;
            // 
            POD_Enum<UnitControllerType::Enum> unitControllerType;
            // 체력창을 유닛의 위치로부터 얼마나 멀리 띄울 것인가?
            POD_Vector2<float> statusBarOffset;
            // 생성될때 걸리는 시간, 생성시 번아웃 효과 연출 시간과 같다.
            float birthTime{ 0.5 };
            bool lingeringCorpse{ false };
            float deathManaReward{ 0 };
            // 사망할 때 번아웃 효과가 연출되는 시간
            float deathBurnOffset{ 0.0 };
            float deathBurnTime{ 0.5 };
            POD_Vector3<float> birthBurnEdgeColor{ 1,1,1 };
            float birthBurnEdgeThickness = 0.01f;
            POD_Vector3<float> deathBurnEdgeColor{ 1,1,1 };
            float deathBurnEdgeThickness = 0.01f;

            float max_Health;

            float m_autoAttackDamage;
            float m_critChance{ 0 };				// 치명타 확률
            float m_critMultiplier{ 1.5f };				// 공격 시 치명타 피해량 배율

            /// Decreasing Damage Elements
            int m_armor{ 5 };
            float m_evasionChance{ 0.0f };				// 회피율
            float m_critResistance{ 0.0f };				// 피격시 치명타 피해 감소율

            float collisionSize = 0.5f;
            float collisionHeight = 0.5f;
            float m_idRadius;
            float m_atkRadius;
            float m_atkCooltime = 2.1f;
            float m_atkRandomDelayMax = 0.125f;
            float m_atkRandomDelayMin = -0.0625f;
            float m_unitSpeed;
            float m_moveAnimationSpeed = 1.0f;
            float rotationSpeed = 180.0f;

            // 유닛의 공격방식, 근거리 혹은 원거리
            POD_Enum<UnitAttackType::Enum> attackType;
            POD_Enum<ProjectileType::Enum> projectileType;
            bool isProjectileHoming{ false };
            std::string projectile_staticFBXName;
            // 발사체가 생성되는 위치
            POD_Vector3<float> projectileOffset;
            // noise가 클수록 착탄지점이 불규칙해진다.
            // 최종적으로 적용되는 착탄지점 노이즈는 공격 대상으로부터의 거리의 제곱에 비례한다.
            float projectileTargetNoise{ 0 };
            float projectileSpeed{ 5 };
            float projectile_scale{ 1.0f };
            float m_attackPreDelay = 0.5f;
            float m_attackPostDelay = 0.5f;
            bool showDmgIndicator{ false };
            POD_Enum<UnitDamageFontType::Enum> dmgIndicatorFont;
            POD_Vector2<float> dmgIndicator_offset;
            float dmgIndicator_offsetNoiseMin = 2.5f;
            float dmgIndicator_offsetNoiseMax = 20.0f;
            float dmgIndicator_travelNoiseMin = 2.5f;
            float dmgIndicator_travelNoiseMax = 40.0f;

            float unit_scale{ 1.0f };

            std::map<std::pair<int, int>, float> animationBlendMap = std::map<std::pair<int, int>, float>();

            TO_JSON(POD_Unit_TemplateData)
                FROM_JSON(POD_Unit_TemplateData)
        };

        class Unit_TemplateData
            :public ITemplateData
        {
            friend class UnitData;
            friend class TemplateDataManager;

        public:
            virtual ~Unit_TemplateData();

            virtual std::string GetDataKey() const override;
            virtual void SetDataResourceName(std::string fbxName) override;
            virtual std::string GetDataResourceName() const override;
            virtual bool EnterDataFromGlobalConstant() override;

            static constexpr float defaultBlendDuration = 0.32f;
            POD_Unit_TemplateData pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            Unit_TemplateData();
            Unit_TemplateData(const Unit_TemplateData& prototype);
            Unit_TemplateData& operator=(const Unit_TemplateData& prototype);
            void FillAnimationBlendMapDefault();
        };
    }
}
