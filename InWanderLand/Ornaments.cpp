#include "Ornaments.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace Application
{
	namespace Editor
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
