/// 2024. 03. 29 김상준
/// Instance 를 생성할 때의 작업을 위한 Command
/// 여기까지 지원하지는 않을 예정

#pragma once

#include "Command.h"

namespace application
{
	namespace editor
	{
		/// 추후 UndoableCommand 로 개선해야 함
		class CreateInstanceCommand
			: public Command
		{
		public:
			COMMAND_SETTING(CommandType::Create)
		};
	}
}

