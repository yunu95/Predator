#include "InWanderLand.h"
#include "Unit_TemplateData.h"

#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "UnitBrush.h"
#include "UnitData.h"

namespace application
{
	namespace editor
	{
		Unit_TemplateData::~Unit_TemplateData()
		{

		}

		std::string Unit_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		void Unit_TemplateData::SetDataResourceName(std::string fbxName)
		{
			if (pod.fbxName == fbxName)
				return;

			pod.fbxName = fbxName;

			palette::UnitBrush::Instance().ChangeBrushResource(this->GetDataKey(), fbxName);

			for (auto each : InstanceManager::GetSingletonInstance().GetList<UnitData>())
			{
				if (each->pod.templateData->GetDataKey() == this->GetDataKey())
				{
					each->OnDataResourceChange(fbxName);
				}
			}

		}

		std::string Unit_TemplateData::GetDataResourceName() const
		{
			return pod.fbxName;
		}

		bool Unit_TemplateData::EnterDataFromGlobalConstant()
		{
			return true;
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
			EnterDataFromGlobalConstant();
			return true;
		}

		Unit_TemplateData::Unit_TemplateData()
			: ITemplateData(), pod()
		{

		}

		Unit_TemplateData::Unit_TemplateData(const Unit_TemplateData& prototype)
			: ITemplateData(prototype), pod(prototype.pod) 
		{
			EnterDataFromGlobalConstant();
		}

		Unit_TemplateData& Unit_TemplateData::operator=(const Unit_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
