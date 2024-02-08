/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 유닛

#pragma once

#include "ITemplateData.h"

namespace application
{
	namespace editor
	{
		class Unit_TemplateData;

		struct POD_Unit_TemplateData
		{
			unsigned int unitType;
			float m_healthPoint;
			int m_manaPoint;

			float m_autoAttackDamage;
			int m_criticalHitProbability;				// 치명타 확률
			float m_criticalHitMultiplier;				// 공격 시 치명타 피해량

			/// Decreasing Damage Elements
			int m_defensePoint;
			int m_dodgeProbability;					// 회피율
			float m_criticalDamageDecreaseMultiplier;	// 피격 시 치명타 피해 감소 배율

			float m_idRadius;
			float m_atkRadius;
			float m_unitSpeed;

			float m_attackDelay;
			TO_JSON(POD_Unit_TemplateData)
			FROM_JSON(POD_Unit_TemplateData)
		};

		class Unit_TemplateData
			:public ITemplateData
		{
			friend class UnitData;
			friend class TemplateDataManager;

		public:
			virtual std::string GetDataKey() const override;

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
