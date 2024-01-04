#include "UnitData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& UnitData::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool UnitData::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* UnitData::GetTemplateData()
		{
			return pod.templateData;
		}

		bool UnitData::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Unit_TemplateData*>(ptr);

			return true;
		}

		IEditableData* UnitData::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance =  imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<UnitData*>(instance)->pod = pod;
			}

			return instance;
		}

		bool UnitData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool UnitData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool UnitData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool UnitData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		UnitData::UnitData()
			: pod()
		{

		}

		UnitData::UnitData(const std::string& name)
			: IEditableData(), pod()
		{
			pod.templateData = static_cast<Unit_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
		}

		UnitData::UnitData(const UnitData& prototype)
			: pod(prototype.pod)
		{

		}

		UnitData& UnitData::operator=(const UnitData& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
