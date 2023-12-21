#include "Units_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		std::string Units_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		bool Units_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Units_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Units_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Units_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Units_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Units_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Units_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Units_TemplateData>>(pod, data["POD"]);

			return true;
		}

		Units_TemplateData::Units_TemplateData()
			: ITemplateData(), pod()
		{
			pod.mypointer = this;
		}

		Units_TemplateData::Units_TemplateData(const Units_TemplateData& prototype)
			: ITemplateData(prototype), pod(prototype.pod) 
		{

		}

		Units_TemplateData& Units_TemplateData::operator=(const Units_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}

