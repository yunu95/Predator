#include "InWanderLand.h"
#include "Ornament_TemplateData.h"

#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "OrnamentBrush.h"
#include "OrnamentData.h"

namespace application
{
	namespace editor
	{
		Ornament_TemplateData::~Ornament_TemplateData()
		{

		}

		std::string Ornament_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		void Ornament_TemplateData::SetDataResourceName(std::string fbxName)
		{
			if (pod.fbxName == fbxName)
				return;

			pod.fbxName = fbxName;

			palette::OrnamentBrush::Instance().ChangeBrushResource(this->GetDataKey(), fbxName);

			for (auto each : InstanceManager::GetSingletonInstance().GetList<OrnamentData>())
			{
				if (each->pod.templateData->GetDataKey() == this->GetDataKey())
				{
					each->OnDataResourceChange(fbxName);
				}
			}
		}

		std::string Ornament_TemplateData::GetDataResourceName() const
		{
			return pod.fbxName;
		}

		bool Ornament_TemplateData::EnterDataFromGlobalConstant()
		{
			auto& data = GlobalConstant::GetSingletonInstance().pod;
			return true;
		}

		bool Ornament_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Ornament_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Ornament_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Ornament_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Ornament_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Ornament_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Ornament_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Ornament_TemplateData>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
			return true;
		}

		Ornament_TemplateData::Ornament_TemplateData()
			: ITemplateData(), pod()
		{

		}

		Ornament_TemplateData::Ornament_TemplateData(const Ornament_TemplateData& prototype)
			: ITemplateData(prototype), pod(prototype.pod)
		{
			EnterDataFromGlobalConstant();
		}

		Ornament_TemplateData& Ornament_TemplateData::operator=(const Ornament_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
