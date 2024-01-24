#include "InWanderLand.h"
#include "MapData.h"
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
            MapData::SaveMapData("TestMap.pmap");
        }
    }
}
