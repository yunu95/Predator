/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 장식물

#pragma once

#include "IEditableData.h"
#include "Ornaments_TemplateData.h"

#include <memory>
#include <string>

namespace application
{
	namespace editor
	{
		class TemplateDataManager;
	}
}

namespace application
{
	namespace editor
	{
		class Ornaments;

		struct POD_Ornaments
		{
			Ornaments_TemplateData* templateData;

			TO_JSON(POD_Ornaments);
			FROM_JSON(POD_Ornaments);
		};

		class Ornaments
			: public IEditableData
		{
			friend class InstanceManager;

		public:
			virtual bool EnterDataFromTemplate() override;
			virtual ITemplateData* GetTemplateData() override;
			virtual bool SetTemplateData(const std::string& dataName) override;
			virtual IEditableData* Clone() const override;

			POD_Ornaments pod;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			static TemplateDataManager& templateDataManager;

			Ornaments();
			Ornaments(const std::string& name);
			Ornaments(const Ornaments& prototype);
			Ornaments& operator=(const Ornaments& prototype);
		};
	}
}
