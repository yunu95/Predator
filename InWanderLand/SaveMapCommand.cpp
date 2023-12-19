#include "SaveMapCommand.h"

#include "MapFileManager.h"

#include "TemplateDataManager.h"
#include "InstanceManager.h"

namespace Application
{
	namespace Editor
	{
		SaveMapCommand::SaveMapCommand()
			: mapFileManager(MapFileManager::GetInstance())
		{
			
		}

		void SaveMapCommand::Execute()
		{
			// test
			//TemplateDataManager::GetInstance().CreateTemplateData("Hero1", IEditableData::DataType::Units);
			//TemplateDataManager::GetInstance().CreateTemplateData("Hero2", IEditableData::DataType::Units);
			//TemplateDataManager::GetInstance().CreateTemplateData("Hero3", IEditableData::DataType::Units);

			//InstanceManager::GetInstance().CreateInstance("Hero1");
			//InstanceManager::GetInstance().CreateInstance("Hero1");
			//InstanceManager::GetInstance().CreateInstance("Hero2");
			//InstanceManager::GetInstance().CreateInstance("Hero2");
			//InstanceManager::GetInstance().CreateInstance("Hero2");
			//InstanceManager::GetInstance().CreateInstance("Hero3");
			//InstanceManager::GetInstance().CreateInstance("Hero3");
			//InstanceManager::GetInstance().CreateInstance("Hero3");
			//InstanceManager::GetInstance().CreateInstance("Hero3");

			mapFileManager.SaveMapFile("TestMap");
		}
	}
}
