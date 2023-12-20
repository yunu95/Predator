#include "Units_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		bool Units_TemplateData::PreEncoding(json& data) const
		{
			FieldEncoding<boost::pfr::tuple_size_v<POD_TemplateData>>(TemplateData::pod, data["POD_Base"]);
			FieldEncoding<boost::pfr::tuple_size_v<POD_Units_TemplateData>>(pod, data["POD"]);
			return true;
		}

		bool Units_TemplateData::PostEncoding(json& data) const
		{
			data["testInt"] = testInt;
			data["testDouble"] = testDouble;
			return true;
		}

		bool Units_TemplateData::PreDecoding(const json& data)
		{
			FieldDecoding<boost::pfr::tuple_size_v<POD_TemplateData>>(TemplateData::pod, data["POD_Base"]);
			FieldDecoding<boost::pfr::tuple_size_v<POD_Units_TemplateData>>(pod, data["POD"]);
			return true;
		}

		bool Units_TemplateData::PostDecoding(const json& data)
		{
			testInt = data["testInt"];
			testDouble = data["testDouble"];
			return true;
		}

		Units_TemplateData::Units_TemplateData()
			: pod(), testInt(100)
		{
			testDouble = 77.7;
		}

		Units_TemplateData::Units_TemplateData(const Units_TemplateData& prototype)
			: TemplateData(prototype), pod(prototype.pod), testInt(prototype.testInt)
		{

		}

		Units_TemplateData& Units_TemplateData::operator=(const Units_TemplateData& prototype)
		{
			TemplateData::operator=(prototype);
			pod = prototype.pod;
			testInt = prototype.testInt;
			return *this;
		}
	}
}

