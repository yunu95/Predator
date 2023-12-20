#include "Terrain_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
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

