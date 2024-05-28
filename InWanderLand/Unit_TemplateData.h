/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 유닛

#pragma once

#include "ITemplateData.h"
#include "GlobalConstant.h"
#include "UnitStatusBarType.h"
#include "PlayerCharacterType.h"

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
            POD_Enum<PlayerCharacterType> playerUnitType{ PlayerCharacterType::None };
            // 화면에 뜨는 체력창의 타입
            POD_Enum<UnitStatusBarType> unitStatusBar{ UnitStatusBarType::ENEMY };
            // 체력창을 유닛의 위치로부터 얼마나 멀리 띄울 것인가?
            POD_Vector2<float> statusBarOffset;
            // 발사체가 생성되는 위치
            POD_Vector3<float> projectileOffset;
            float projectileSpeed{ 5 };
            // 생성될때 걸리는 시간, 생성시 번아웃 효과 연출 시간과 같다.
            float spawnTime{ 0.5 };
            // 사망할 때 번아웃 효과가 연출되는 시간
            float deathBurnTime{ 0.5 };

            int unitType;
            float max_Health;

            float m_autoAttackDamage;
            int m_criticalHitProbability;				// 치명타 확률
            float m_criticalHitMultiplier;				// 공격 시 치명타 피해량

            /// Decreasing Damage Elements
            int m_defensePoint;
            float m_dodgeProbability;					// 회피율
            float m_criticalDamageDecreaseMultiplier;	// 피격 시 치명타 피해 감소 배율

            float collisionSize = 0.5f;
            float m_idRadius;
            float m_atkRadius;
            float m_unitSpeed;

            float m_attackEngageDelay;
            int m_attackTimingFrame;
            float m_attackOffset;

            int isEliteMonster;
            int chessType;

            float unit_scale{ 1.0f };

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
        };
    }
}
