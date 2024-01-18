#ifdef EDITOR
#include "InWanderLand.h"
#include "Unit_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		std::string Unit_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		bool Unit_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Unit_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Unit_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Unit_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);

			return true;
		}

		Unit_TemplateData::Unit_TemplateData()
			: ITemplateData(), pod()
		{

		}

		Unit_TemplateData::Unit_TemplateData(const Unit_TemplateData& prototype)
			: ITemplateData(prototype), pod(prototype.pod) 
		{

		}

		Unit_TemplateData& Unit_TemplateData::operator=(const Unit_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
#endif
