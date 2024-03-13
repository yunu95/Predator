#include "InWanderLand.h"
#include "Light_TemplateData.h"

#include "TemplateDataManager.h"
#include "LightBrush.h"

namespace application
{
	namespace editor
	{
		std::string Light_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		bool Light_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Light_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Light_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Light_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Light_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Light_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Light_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Light_TemplateData>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
			return true;
		}

		Light_TemplateData::Light_TemplateData()
			: ITemplateData(), pod()
		{
			EnterDataFromGlobalConstant();
		}

		Light_TemplateData::Light_TemplateData(const Light_TemplateData& prototype)
			:ITemplateData(prototype), pod(prototype.pod)
		{
			EnterDataFromGlobalConstant();
		}

		Light_TemplateData& Light_TemplateData::operator=(const Light_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}

