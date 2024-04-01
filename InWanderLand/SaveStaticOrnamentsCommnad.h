/// 2024. 03. 05 김상준
/// Ornament 정보를 Unreal 용 Json 으로
/// Export 하는 Command

#pragma once

#include "Command.h"

namespace application
{
	namespace editor
	{
		class MapFileManager;
	}
}

namespace application
{
	namespace editor
	{
		class SaveStaticOrnamentsCommand
			: public Command
		{
		public:
			COMMAND_SETTING(CommandType::SaveOrnaments)

			SaveStaticOrnamentsCommand();

			virtual void Execute() override;

		private:
			MapFileManager& mapFileManager;
		};

	}
}

