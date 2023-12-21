#include "Ornaments.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& Ornaments::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool Ornaments::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* Ornaments::GetTemplateData()
		{
			return pod.templateData;
		}

		bool Ornaments::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Ornaments_TemplateData*>(ptr);

			return true;
		}

		IEditableData* Ornaments::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<Ornaments*>(instance)->pod = pod;
			}

			return instance;
		}

		bool Ornaments::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Ornaments>>(pod, data["POD"]);

			return true;
		}

		bool Ornaments::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Ornaments>>(pod, data["POD"]);

			return true;
		}

		bool Ornaments::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Ornaments>>(pod, data["POD"]);

			return true;
		}

		bool Ornaments::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Ornaments>>(pod, data["POD"]);

			return true;
		}

		Ornaments::Ornaments()
			: pod()
		{

		}

		Ornaments::Ornaments(const std::string& name)
			: pod()
		{
			pod.templateData = static_cast<Ornaments_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
		}

		Ornaments::Ornaments(const Ornaments& prototype)
			: pod(prototype.pod)
		{

		}

		Ornaments& Ornaments::operator=(const Ornaments& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
