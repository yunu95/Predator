#include "LoadStaticOrnamentsCommand.h"

#include "InWanderLand.h"
#include "MapFileManager.h"
#include "FileSystem.h"

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
			std::filesystem::path filepath = fileSystem::LoadFileDialog("Unreal Save File (*.punreal)\0*.punreal\0");

			if (filepath.empty())
				return;

			if (!filepath.has_extension())
				filepath += ".punreal";
	
			MapFileManager::GetSingletonInstance().LoadStaticOrnaments(filepath.string());
		}
	}
}

