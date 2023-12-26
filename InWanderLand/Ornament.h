/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 장식물

#pragma once

#include "IEditableData.h"
#include "Ornament_TemplateData.h"

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
		class Ornament;

		struct POD_Ornament
		{
			Ornament_TemplateData* templateData;

			TO_JSON(POD_Ornament);
			FROM_JSON(POD_Ornament);
		};

		class Ornament
			: public IEditableData
		{
			friend class InstanceManager;

		public:
			virtual bool EnterDataFromTemplate() override;
			virtual ITemplateData* GetTemplateData() override;
			virtual bool SetTemplateData(const std::string& dataName) override;
			virtual IEditableData* Clone() const override;

			POD_Ornament pod;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			static TemplateDataManager& templateDataManager;

			Ornament();
			Ornament(const std::string& name);
			Ornament(const Ornament& prototype);
			Ornament& operator=(const Ornament& prototype);
		};
	}
}
