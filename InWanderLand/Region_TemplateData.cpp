#include "InWanderLand.h"
#include "Region_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		std::string Region_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		bool Region_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Region_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Region_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Region_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Region_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Region_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Region_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Region_TemplateData>>(pod, data["POD"]);

			return true;
		}

		Region_TemplateData::Region_TemplateData()
			: ITemplateData(), pod()
		{

		}

		Region_TemplateData::Region_TemplateData(const Region_TemplateData& prototype)
			:ITemplateData(prototype), pod(prototype.pod)
		{		

		}

		Region_TemplateData& Region_TemplateData::operator=(const Region_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}