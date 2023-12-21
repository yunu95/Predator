#include "Terrain.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& Terrain::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool Terrain::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* Terrain::GetTemplateData()
		{
			return pod.templateData;
		}

		bool Terrain::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Terrain_TemplateData*>(ptr);

			return true;
		}

		IEditableData* Terrain::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<Terrain*>(instance)->pod = pod;
			}

			return instance;
		}

		bool Terrain::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		bool Terrain::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		bool Terrain::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		bool Terrain::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		Terrain::Terrain()
			: pod()
		{

		}

		Terrain::Terrain(const std::string& name)
			: IEditableData(), pod()
		{
			pod.templateData = static_cast<Terrain_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
		}

		Terrain::Terrain(const Terrain& prototype)
			: pod(prototype.pod)
		{

		}

		Terrain& Terrain::operator=(const Terrain& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
