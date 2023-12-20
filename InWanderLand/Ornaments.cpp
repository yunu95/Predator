#include "Ornaments.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateData* Ornaments::GetTemplateData()
		{
			return templateData;
		}

		bool Ornaments::SetTemplateData(const std::string& dataName)
		{
			auto ptr = TemplateDataManager::GetInstance().GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			templateData = static_cast<Ornaments_TemplateData*>(ptr);

			return true;
		}

		IEditableData* Ornaments::Clone() const
		{
			auto& imanager = InstanceManager::GetInstance();
			auto instance = imanager.CreateInstance(templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<Ornaments*>(instance)->instanceData = instanceData;
			}

			return instance;
		}

		bool Ornaments::PreEncoding(json& data) const
		{
			if (!instanceData.PreEncoding(data["instance_data"]))
			{
				return false;
			}

			return true;
		}

		bool Ornaments::PostEncoding(json& data) const
		{
			if (!instanceData.PostEncoding(data["instance_data"]))
			{
				return false;
			}

			data["template_data"] = UUID_To_String(templateData->GetUUID());

			return true;
		}

		bool Ornaments::PreDecoding(const json& data)
		{
			if (!instanceData.PreDecoding(data["instance_data"]))
			{
				return false;
			}

			return true;
		}

		bool Ornaments::PostDecoding(const json& data)
		{
			if (!instanceData.PostDecoding(data["instance_data"]))
			{
				return false;
			}

			auto& tdManager = TemplateDataManager::GetInstance();

			UUID uuid = String_To_UUID(data["template_data"]);

			return SetTemplateData(tdManager.GetDataKey(uuid));
		}

		Ornaments::Ornaments()
			: instanceData(), templateData(nullptr)
		{

		}

		Ornaments::Ornaments(const std::string& name)
			: instanceData(), templateData()
		{
			auto& manager = TemplateDataManager::GetInstance();
			templateData = static_cast<Ornaments_TemplateData*>(manager.GetTemplateData(name));
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
