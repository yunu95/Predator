/// 2023. 12. 08 김상준
/// 에디터의 현재 맵 상태를 저장하는 커맨드

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

