/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 유닛

#pragma once

#include "ITemplateData.h"

#include <string>

namespace application
{
	namespace editor
	{
		class Unit_TemplateData;

		struct POD_Unit_TemplateData
		{
			std::string thumbnailPath = "ImageButtons/Unit_Default.png";

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
