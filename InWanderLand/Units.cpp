#include "Units.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateData* Units::GetTemplateData()
		{
			return templateData;
		}

		bool Units::SetTemplateData(const std::string& dataName)
		{
			auto ptr = TemplateDataManager::GetInstance().GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			templateData = static_cast<Units_TemplateData*>(ptr);

			return true;
		}

		IEditableData* Units::Clone() const
		{
			auto& imanager = InstanceManager::GetInstance();
			auto instance =  imanager.CreateInstance(templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<Units*>(instance)->instanceData = instanceData;
			}

			return instance;
		}

		bool Units::PreEncoding(json& data) const
		{
			if (!instanceData.PreEncoding(data["instance_data"]))
			{
				return false;
			}

			return true;
		}

		bool Units::PostEncoding(json& data) const
		{
			if (!instanceData.PostEncoding(data["instance_data"]))
			{
				return false;
			}

			data["template_data"] = UUID_To_String(templateData->GetUUID());

			return true;
		}

		bool Units::PreDecoding(const json& data)
		{
			if (!instanceData.PreDecoding(data["instance_data"]))
			{
				return false;
			}

			return true;
		}

		bool Units::PostDecoding(const json& data)
		{
			if (!instanceData.PostDecoding(data["instance_data"]))
			{
				return false;
			}

			auto& tdManager = TemplateDataManager::GetInstance();

			UUID uuid = String_To_UUID(data["template_data"]);

			return SetTemplateData(tdManager.GetDataKey(uuid));
		}

		Units::Units()
			: instanceData(), templateData(nullptr)
		{

		}

		Units::Units(const std::string& name)
			: instanceData(), templateData()
		{
			auto& manager = TemplateDataManager::GetInstance();
			templateData = static_cast<Units_TemplateData*>(manager.GetTemplateData(name));
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
