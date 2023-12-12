#include "Terrain.h"

#include "TemplateDataManager.h"

namespace Application
{
	namespace Editor
	{
		std::shared_ptr<IEditableData> Terrain::Clone() const
		{
			return std::shared_ptr<IEditableData>(new Terrain(*this));
		}

		bool Terrain::PreEncoding(json& data) const
		{
			return true;
		}

		bool Terrain::PostEncoding(json& data) const
		{
			return true;
		}

		bool Terrain::PreDecoding(const json& data)
		{
			return true;
		}

		bool Terrain::PostDecoding(const json& data)
		{
			return true;
		}

		Terrain::Terrain(const std::string& name)
			: instanceData(), templateData()
		{
			auto& manager = TemplateDataManager::GetInstance();
			if (!manager.CreateTemplateData(name, IEditableData::DataType::Terrain))
			{
				// 이미 해당하는 이름의 데이터가 있는 경우, templateData 가 nullptr
				return;
			}

			templateData = std::static_pointer_cast<Terrain_TemplateData>(manager.GetTemplateData(name));
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
