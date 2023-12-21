#include "Units.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& Units::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool Units::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	
			pod.currentHP = pod.templateData->pod.maxHP;

			return true;
		}

		ITemplateData* Units::GetTemplateData()
		{
			return pod.templateData;
		}

		bool Units::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Units_TemplateData*>(ptr);

			return true;
		}

		IEditableData* Units::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance =  imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<Units*>(instance)->pod = pod;
			}

			return instance;
		}

		bool Units::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Units>>(pod, data["POD"]);

			return true;
		}

		bool Units::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Units>>(pod, data["POD"]);

			return true;
		}

		bool Units::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Units>>(pod, data["POD"]);

			return true;
		}

		bool Units::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Units>>(pod, data["POD"]);

			return true;
		}

		Units::Units()
			: pod()
		{
			pod.testptr = this;
		}

		Units::Units(const std::string& name)
			: IEditableData(), pod()
		{
			pod.templateData = static_cast<Units_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();

			pod.testptr = this;
		}

		Units::Units(const Units& prototype)
			: pod(prototype.pod)
		{

		}

		Units& Units::operator=(const Units& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
