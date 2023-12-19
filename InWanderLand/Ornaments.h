/// 2023. 11. 23 김상준
/// EditableData 의 구체화된 클래스
/// 장식물

#pragma once

#include "IEditableData.h"
#include "Ornaments_InstanceData.h"
#include "Ornaments_TemplateData.h"

#include <memory>
#include <string>

namespace Application
{
	namespace Editor
	{
		class Ornaments
			: public IEditableData
		{
			friend class InstanceManager;

		public:
			virtual TemplateData* GetTemplateData() override;
			virtual bool SetTemplateData(const std::string& dataName) override;
			virtual IEditableData* Clone() const override;

			Ornaments_InstanceData instanceData;
			Ornaments_TemplateData* templateData;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			Ornaments();
			Ornaments(const std::string& name);
			Ornaments(const Ornaments& prototype);
			Ornaments& operator=(const Ornaments& prototype);
		};
	}
}
