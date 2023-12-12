#include "Ornaments.h"

#include "TemplateDataManager.h"

namespace Application
{
	namespace Editor
	{
		std::shared_ptr<IEditableData> Ornaments::Clone() const
		{
			return std::shared_ptr<IEditableData>(new Ornaments(*this));
		}

		bool Ornaments::PreEncoding(json& data) const
		{
			return true;
		}

		bool Ornaments::PostEncoding(json& data) const
		{
			return true;
		}

		bool Ornaments::PreDecoding(const json& data)
		{
			return true;
		}

		bool Ornaments::PostDecoding(const json& data)
		{
			return true;
		}

		Ornaments::Ornaments(const std::string& name)
			: instanceData(), templateData()
		{
			auto& manager = TemplateDataManager::GetInstance();
			if (!manager.CreateTemplateData(name, IEditableData::DataType::Ornaments))
			{
				// 이미 해당하는 이름의 데이터가 있는 경우, templateData 가 nullptr
				return;
			}

			templateData = std::static_pointer_cast<Ornaments_TemplateData>(manager.GetTemplateData(name));
			instanceData.EnterDataFromTemplate(templateData);
		}

		Ornaments::Ornaments(const Ornaments& prototype)
			: instanceData(prototype.instanceData), templateData(prototype.templateData)
		{
			
		}

		Ornaments& Ornaments::operator=(const Ornaments& prototype)
		{
			instanceData = prototype.instanceData;
			templateData = prototype.templateData;
			return *this;
		}
	}
}
