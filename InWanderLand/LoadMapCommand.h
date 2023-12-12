/// 2023. 12. 08 김상준
/// 맵 파일을 에디터에 불러오는 커맨드

#pragma once

#include "Command.h"

namespace Application
{
	namespace Editor
	{
		class MapFileManager;
	}
}

namespace Application
{
	namespace Editor
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

