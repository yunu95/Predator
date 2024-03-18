#include "SaveStaticOrnamentsCommnad.h"

#include "InWanderLand.h"
#include "MapFileManager.h"
#include "FileSystem.h"

namespace application
{
	namespace editor
	{
		SaveStaticOrnamentsCommand::SaveStaticOrnamentsCommand()
			: mapFileManager(MapFileManager::GetSingletonInstance())
		{

		}

		void SaveStaticOrnamentsCommand::Execute()
		{
			std::filesystem::path filepath = fileSystem::SaveFileDialog("Unreal Save File (*.punreal)\0*.punreal\0");

			if (filepath.empty())
				return;

			if (!filepath.has_extension())
				filepath += ".punreal";

			mapFileManager.SaveStaticOrnaments(filepath.string());
		}
	}
}

