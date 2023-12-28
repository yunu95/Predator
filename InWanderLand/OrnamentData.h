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
		class OrnamentData;

		struct POD_Ornament
		{
			Ornament_TemplateData* templateData;

			TO_JSON(POD_Ornament);
			FROM_JSON(POD_Ornament);
		};

		class OrnamentData
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

			OrnamentData();
			OrnamentData(const std::string& name);
			OrnamentData(const OrnamentData& prototype);
			OrnamentData& operator=(const OrnamentData& prototype);
		};
	}
}
