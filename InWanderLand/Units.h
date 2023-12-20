/// 2023. 11. 23 김상준
/// EditableData 의 구체화된 클래스
/// 유닛

#pragma once

#include "IEditableData.h"
#include "Units_InstanceData.h"
#include "Units_TemplateData.h"

#include <memory>
#include <string>

namespace application
{
	namespace editor
	{
		class Units
			: public IEditableData
		{
			friend class InstanceManager;

		public:
			virtual TemplateData* GetTemplateData() override;
			virtual bool SetTemplateData(const std::string& dataName) override;
			virtual IEditableData* Clone() const override;

			Units_InstanceData instanceData;
			Units_TemplateData* templateData;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			Units();
			Units(const std::string& name);
			Units(const Units& prototype);
			Units& operator=(const Units& prototype);
		};
	}
}
