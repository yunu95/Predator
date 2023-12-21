/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 유닛

#pragma once

#include "ITemplateData.h"

namespace application
{
	namespace editor
	{
		class Units_TemplateData;

		struct POD_Units_TemplateData
		{
			TO_JSON(POD_Units_TemplateData);
			FROM_JSON(POD_Units_TemplateData);

			int maxHP = 50;
			ITemplateData* mypointer = nullptr;
		};

		class Units_TemplateData
			:public ITemplateData
		{
			friend class Units;
			friend class TemplateDataManager;

		public:
			virtual std::string GetDataKey() const override;

			POD_Units_TemplateData pod;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			Units_TemplateData();
			Units_TemplateData(const Units_TemplateData& prototype);
			Units_TemplateData& operator=(const Units_TemplateData& prototype);
		};
	}
}
