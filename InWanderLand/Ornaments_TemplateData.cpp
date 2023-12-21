#include "Ornaments_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		std::string Ornaments_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		bool Ornaments_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Ornaments_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Ornaments_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Ornaments_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Ornaments_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Ornaments_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Ornaments_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Ornaments_TemplateData>>(pod, data["POD"]);

			return true;
		}

		Ornaments_TemplateData::Ornaments_TemplateData()
			: ITemplateData(), pod()
		{

		}

		Ornaments_TemplateData::Ornaments_TemplateData(const Ornaments_TemplateData& prototype)
			: ITemplateData(prototype), pod(prototype.pod)
		{

		}

		Ornaments_TemplateData& Ornaments_TemplateData::operator=(const Ornaments_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}

