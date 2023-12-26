#include "Unit.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& Unit::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool Unit::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* Unit::GetTemplateData()
		{
			return pod.templateData;
		}

		bool Unit::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Unit_TemplateData*>(ptr);

			return true;
		}

		IEditableData* Unit::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance =  imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<Unit*>(instance)->pod = pod;
			}

			return instance;
		}

		bool Unit::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool Unit::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool Unit::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool Unit::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		Unit::Unit()
			: pod()
		{

		}

		Unit::Unit(const std::string& name)
			: IEditableData(), pod()
		{
			pod.templateData = static_cast<Unit_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
		}

		Unit::Unit(const Unit& prototype)
			: pod(prototype.pod)
		{

		}

		Unit& Unit::operator=(const Unit& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
