/// 2023. 12. 08 김상준
/// 에디터의 현재 맵 상태를 저장하는 커맨드

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
		class SaveMapCommand
			: public Command
		{
		public:
			SaveMapCommand();

			virtual void Execute() override;

		private:
			MapFileManager& mapFileManager;
		};
	}
}
#endif
