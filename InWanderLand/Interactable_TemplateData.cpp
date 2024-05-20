#include "InWanderLand.h"
#include "Interactable_TemplateData.h"

#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "InteractableBrush.h"
#include "InteractableData.h"

namespace application
{
	namespace editor
	{
		std::vector<std::string> Interactable_TemplateData::interactableFBXNameList = { "Trigger_Cube", "Trigger_Sphere", "SM_Chess_Bishop", "SM_Chess_Pawn", "SM_Chess_Rook", "SM_Spike01"};

		Interactable_TemplateData::~Interactable_TemplateData()
		{

		}

		std::string Interactable_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		void Interactable_TemplateData::SetDataResourceName(std::string fbxName)
		{
			if (pod.fBXName == fbxName)
				return;

			pod.fBXName = fbxName;

			palette::InteractableBrush::Instance().ChangeBrushResource(this->GetDataKey(), fbxName);

			for (auto each : InstanceManager::GetSingletonInstance().GetList<InteractableData>())
			{
				if (each->pod.templateData->GetDataKey() == this->GetDataKey())
				{
					each->OnDataResourceChange(fbxName);
				}
			}
		}

		std::string Interactable_TemplateData::GetDataResourceName() const
		{
			return pod.fBXName;
		}

		bool Interactable_TemplateData::EnterDataFromGlobalConstant()
		{
			auto& data = GlobalConstant::GetSingletonInstance().pod;
			return true;
		}

		bool Interactable_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Interactable_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Interactable_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Interactable_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Interactable_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Interactable_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Interactable_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Interactable_TemplateData>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
			return true;
		}

		Interactable_TemplateData::Interactable_TemplateData()
			: ITemplateData(), pod()
		{
			EnterDataFromGlobalConstant();
		}

		Interactable_TemplateData::Interactable_TemplateData(const Interactable_TemplateData& prototype)
			: ITemplateData(prototype), pod(prototype.pod)
		{
			EnterDataFromGlobalConstant();
		}

		Interactable_TemplateData& Interactable_TemplateData::operator=(const Interactable_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
