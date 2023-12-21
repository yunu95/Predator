/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 유닛

#pragma once

#include "IEditableData.h"
#include "Units_TemplateData.h"

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
		class Units;

		struct POD_Units
		{
			Units_TemplateData* templateData = nullptr;

			TO_JSON(POD_Units);
			FROM_JSON(POD_Units);

			std::string testName = "T1";
			int currentHP = 100000;
			IEditableData* testptr = nullptr;
		};

		class Units
			: public IEditableData
		{
			friend class InstanceManager;

		public:
			virtual bool EnterDataFromTemplate() override;
			virtual ITemplateData* GetTemplateData() override;
			virtual bool SetTemplateData(const std::string& dataName) override;
			virtual IEditableData* Clone() const override;

			POD_Units pod;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			static TemplateDataManager& templateDataManager;

			Units();
			Units(const std::string& name);
			Units(const Units& prototype);
			Units& operator=(const Units& prototype);
		};
	}
}
