/// 2023. 12. 08 김상준
/// 맵 파일을 에디터에 불러오는 커맨드

#pragma once

#ifdef EDITOR
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
		class LoadMapCommand
			: public Command
		{
		public:
			LoadMapCommand();

			virtual void Execute() override;

		private:
			MapFileManager& mapFileManager;
		};
	}
}
#endif
