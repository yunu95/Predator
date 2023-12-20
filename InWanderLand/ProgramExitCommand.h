/// 2023. 10. 20 김상준
/// 애플리케이션 종료를 위한 커맨드

#pragma once

#include "Command.h"

namespace application
{
	class Application;
}

namespace application
{
	namespace editor
	{
		class ProgramExitCommand
			: public Command
		{
		public:
			ProgramExitCommand();

			virtual void Execute() override;
			
		private:
			Application& app;
		};
	}
}

