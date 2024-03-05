/// 2023. 12. 08 김상준
/// 에디터의 현재 맵 상태를 저장하는 커맨드

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
		class SaveMapCommand
			: public Command
		{
		public:
			SaveMapCommand(bool saveAs = false);

			virtual void Execute() override;

		private:			
			bool saveAs = false;
			MapFileManager& mapFileManager;
		};
	}
}
