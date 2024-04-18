/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 유닛

#pragma once

#include "ITemplateData.h"
#include "GlobalConstant.h"

#include <string>

namespace application
{
	namespace editor
	{
		class Unit_TemplateData;

		struct POD_Unit_TemplateData
		{
			std::string skinnedFBXName = std::string();

			int unitType;
			float m_healthPoint;

			float m_autoAttackDamage;
			int m_criticalHitProbability;				// 치명타 확률
			float m_criticalHitMultiplier;				// 공격 시 치명타 피해량

			/// Decreasing Damage Elements
			int m_defensePoint;
			float m_dodgeProbability;					// 회피율
			float m_criticalDamageDecreaseMultiplier;	// 피격 시 치명타 피해 감소 배율

			float m_idRadius;
			float m_atkRadius;
			float m_unitSpeed;

			float m_attackEngageDelay;
			int m_attackTimingFrame;
			float m_attackOffset;

			int isEliteMonster;
			int chessType;

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
