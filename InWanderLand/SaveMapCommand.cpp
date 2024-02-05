#include "InWanderLand.h"
#include "SaveMapCommand.h"

#include "MapFileManager.h"

namespace application
{
    namespace editor
    {
        SaveMapCommand::SaveMapCommand()
            : mapFileManager(MapFileManager::GetSingletonInstance())
        {

        }

        void SaveMapCommand::Execute()
        {
			MapFileManager::GetSingletonInstance().SaveMapFile("TestMap.pmap");
        }
    }
}
