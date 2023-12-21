/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 장식물

#pragma once

#include "ITemplateData.h"

namespace application
{
	namespace editor
	{
		class Ornaments_TemplateData;

		struct POD_Ornaments_TemplateData
		{
			TO_JSON(POD_Ornaments_TemplateData);
			FROM_JSON(POD_Ornaments_TemplateData);
		};

		class Ornaments_TemplateData
			:public ITemplateData
		{
			friend class Ornaments;
			friend class TemplateDataManager;

		public:
			virtual std::string GetDataKey() const override;

			POD_Ornaments_TemplateData pod;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			Ornaments_TemplateData();
			Ornaments_TemplateData(const Ornaments_TemplateData& prototype);
			Ornaments_TemplateData& operator=(const Ornaments_TemplateData& prototype);
		};
	}
}

