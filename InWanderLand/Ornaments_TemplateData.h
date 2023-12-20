/// 2023. 11. 23 김상준
/// TemplateData 의 구체화된 클래스
/// 장식물

#pragma once

#include "TemplateData.h"

namespace application
{
	namespace editor
	{
		class Ornaments_TemplateData
			:public TemplateData
		{
			friend class Ornaments;
			friend class TemplateDataManager;

		public:

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

