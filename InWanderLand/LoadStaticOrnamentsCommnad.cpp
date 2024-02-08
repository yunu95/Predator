#include "LoadStaticOrnamentsCommand.h"

#include "InWanderLand.h"
#include "MapFileManager.h"

namespace application
{
	namespace editor
	{
		LoadStaticOrnamentsCommand::LoadStaticOrnamentsCommand()
			: mapFileManager(MapFileManager::GetSingletonInstance())
		{

		}

		void LoadStaticOrnamentsCommand::Execute()
		{
			MapFileManager::GetSingletonInstance().LoadStaticOrnaments("TestOrnaments.punreal");
		}
	}
}

