/// 2024. 02. 05 김상준
/// Unreal 에서 작업했던 정보들을 저장한 Json 파일로부터
/// Ornament 를 추가하는 Command

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
		class LoadStaticOrnamentsCommand
			: public Command
		{
		public:
			COMMAND_SETTING(CommandType::LoadOrnaments)

			LoadStaticOrnamentsCommand();

			virtual void Execute() override;

		private:
			MapFileManager& mapFileManager;
		};

	}
}

