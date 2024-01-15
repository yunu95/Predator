#include "InWanderLand.h"
#include "TerrainData.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& TerrainData::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool TerrainData::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* TerrainData::GetTemplateData()
		{
			return pod.templateData;
		}

		bool TerrainData::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Terrain_TemplateData*>(ptr);

			return true;
		}

		IEditableData* TerrainData::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<TerrainData*>(instance)->pod = pod;
			}

			return instance;
		}

		bool TerrainData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		bool TerrainData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		bool TerrainData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		bool TerrainData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Terrain>>(pod, data["POD"]);

			return true;
		}

		TerrainData::TerrainData()
			: pod()
		{

		}

		TerrainData::TerrainData(const std::string& name)
			: IEditableData(), pod()
		{
			pod.templateData = static_cast<Terrain_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
		}

		TerrainData::TerrainData(const TerrainData& prototype)
			: pod(prototype.pod)
		{

		}

		TerrainData& TerrainData::operator=(const TerrainData& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
