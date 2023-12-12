#include "Units.h"

#include "TemplateDataManager.h"

namespace Application
{
	namespace Editor
	{
		std::shared_ptr<IEditableData> Units::Clone() const
		{
			return std::shared_ptr<IEditableData>(new Units(*this));
		}

		bool Units::PreEncoding(json& data) const
		{
			instanceData.PreEncoding(data);
			templateData->PreEncoding(data);

			return true;
		}

		bool Units::PostEncoding(json& data) const
		{
			instanceData.PostEncoding(data);
			templateData->PostEncoding(data);

			return true;
		}

		bool Units::PreDecoding(const json& data)
		{
			instanceData.PreDecoding(data);
			templateData->PreDecoding(data);

			return true;
		}

		bool Units::PostDecoding(const json& data)
		{
			instanceData.PostDecoding(data);
			templateData->PostDecoding(data);

			return true;
		}

		Units::Units(const std::string& name)
			: instanceData(), templateData()
		{
			auto& manager = TemplateDataManager::GetInstance();
			if (!manager.CreateTemplateData(name, IEditableData::DataType::Units))
			{
				// 이미 해당하는 이름의 데이터가 있는 경우, templateData 가 nullptr
				return;
			}

			templateData = std::static_pointer_cast<Units_TemplateData>(manager.GetTemplateData(name));
			instanceData.EnterDataFromTemplate(templateData);
		}

		Units::Units(const Units& prototype)
			: instanceData(prototype.instanceData), templateData(prototype.templateData)
		{

		}

		Units& Units::operator=(const Units& prototype)
		{
			instanceData = prototype.instanceData;
			templateData = prototype.templateData;
			return *this;
		}
	}
}
