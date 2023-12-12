#include "Terrain_TemplateData.h"

#include "TemplateDataManager.h"

namespace Application
{
	namespace Editor
	{
		std::shared_ptr<TemplateData> Terrain_TemplateData::Clone() const
		{
			return std::shared_ptr<TemplateData>(new Terrain_TemplateData(*this));
		}

		bool Terrain_TemplateData::PreEncoding(json& data) const
		{
			return true;
		}

		bool Terrain_TemplateData::PostEncoding(json& data) const
		{
			return true;
		}

		bool Terrain_TemplateData::PreDecoding(const json& data)
		{
			return true;
		}

		bool Terrain_TemplateData::PostDecoding(const json& data)
		{
			return true;
		}

		Terrain_TemplateData::Terrain_TemplateData()
		{

		}

		Terrain_TemplateData::Terrain_TemplateData(const Terrain_TemplateData& prototype)
			:TemplateData(prototype)
		{

		}

		Terrain_TemplateData& Terrain_TemplateData::operator=(const Terrain_TemplateData& prototype)
		{
			TemplateData::operator=(prototype);
			return *this;
		}
	}
}

