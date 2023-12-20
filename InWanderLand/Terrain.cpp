#include "Terrain.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateData* Terrain::GetTemplateData()
		{
			return templateData;
		}

		bool Terrain::SetTemplateData(const std::string& dataName)
		{
			auto ptr = TemplateDataManager::GetInstance().GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			templateData = static_cast<Terrain_TemplateData*>(ptr);

			return true;
		}

		IEditableData* Terrain::Clone() const
		{
			auto& imanager = InstanceManager::GetInstance();
			auto instance = imanager.CreateInstance(templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<Terrain*>(instance)->instanceData = instanceData;
			}

			return instance;
		}

		bool Terrain::PreEncoding(json& data) const
		{
			if (!instanceData.PreEncoding(data["instance_data"]))
			{
				return false;
			}

			return true;
		}

		bool Terrain::PostEncoding(json& data) const
		{
			if (!instanceData.PostEncoding(data["instance_data"]))
			{
				return false;
			}

			data["template_data"] = UUID_To_String(templateData->GetUUID());

			return true;
		}

		bool Terrain::PreDecoding(const json& data)
		{
			if (!instanceData.PreDecoding(data["instance_data"]))
			{
				return false;
			}

			return true;
		}

		bool Terrain::PostDecoding(const json& data)
		{
			if (!instanceData.PostDecoding(data["instance_data"]))
			{
				return false;
			}

			auto& tdManager = TemplateDataManager::GetInstance();

			UUID uuid = String_To_UUID(data["template_data"]);

			return SetTemplateData(tdManager.GetDataKey(uuid));
		}

		Terrain::Terrain()
			: instanceData(), templateData(nullptr)
		{

		}

		Terrain::Terrain(const std::string& name)
			: instanceData(), templateData()
		{
			auto& manager = TemplateDataManager::GetInstance();
			templateData = static_cast<Terrain_TemplateData*>(manager.GetTemplateData(name));
			instanceData.EnterDataFromTemplate(templateData);
		}

		Terrain::Terrain(const Terrain& prototype)
			: instanceData(prototype.instanceData), templateData(prototype.templateData)
		{

		}

		Terrain& Terrain::operator=(const Terrain& prototype)
		{
			instanceData = prototype.instanceData;
			templateData = prototype.templateData;
			return *this;
		}
	}
}
