#include "InWanderLand.h"
#include "Camera_TemplateData.h"

#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		std::string Camera_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		bool Camera_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Camera_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Camera_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Camera_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Camera_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Camera_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Camera_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Camera_TemplateData>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
			return true;
		}

		Camera_TemplateData::Camera_TemplateData()
			: ITemplateData(), pod()
		{

		}

		Camera_TemplateData::Camera_TemplateData(const Camera_TemplateData& prototype)
			:ITemplateData(prototype), pod(prototype.pod)
		{
			EnterDataFromGlobalConstant();
		}

		Camera_TemplateData& Camera_TemplateData::operator=(const Camera_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}

