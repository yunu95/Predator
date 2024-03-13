#include "InWanderLand.h"
#include "Terrain_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		std::string Terrain_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		bool Terrain_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Terrain_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Terrain_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Terrain_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Terrain_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Terrain_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Terrain_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Terrain_TemplateData>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
			return true;
		}

		Terrain_TemplateData::Terrain_TemplateData()
			: ITemplateData(), pod()
		{
			EnterDataFromGlobalConstant();
		}

		Terrain_TemplateData::Terrain_TemplateData(const Terrain_TemplateData& prototype)
			:ITemplateData(prototype), pod(prototype.pod)
		{		
			EnterDataFromGlobalConstant();
		}

		Terrain_TemplateData& Terrain_TemplateData::operator=(const Terrain_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
