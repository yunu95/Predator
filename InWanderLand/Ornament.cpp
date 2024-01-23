#include "InWanderLand.h"
#include "OrnamentData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& OrnamentData::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool OrnamentData::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* OrnamentData::GetTemplateData()
		{
			return pod.templateData;
		}

		bool OrnamentData::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Ornament_TemplateData*>(ptr);

			return true;
		}

		IEditableData* OrnamentData::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<OrnamentData*>(instance)->pod = pod;
			}

			return instance;
		}

		bool OrnamentData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);

			return true;
		}

		bool OrnamentData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);

			return true;
		}

		bool OrnamentData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);

			return true;
		}

		bool OrnamentData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Ornament>>(pod, data["POD"]);

			return true;
		}

		OrnamentData::OrnamentData()
			: pod()
		{

		}

		OrnamentData::OrnamentData(const std::string& name)
			: pod()
		{
			pod.templateData = static_cast<Ornament_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
		}

		OrnamentData::OrnamentData(const OrnamentData& prototype)
			: pod(prototype.pod)
		{

		}

		OrnamentData& OrnamentData::operator=(const OrnamentData& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
